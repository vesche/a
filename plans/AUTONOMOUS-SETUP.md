# Running the Self-Improvement Agent Autonomously

Step-by-step guide for setting up unsupervised, continuous operation of the `a` self-improvement agent.

---

## What the Agent Does

Each cycle, the agent:

1. Creates a feature branch `self-improve/run-N`
2. Scans all 61 standard library modules
3. Profiles each module using its test suite
4. Identifies hot functions owned by the module (not the test harness)
5. Applies safe mechanical transforms (inline small hot functions)
6. Validates every change compiles and passes tests; rolls back on failure
7. Commits each validated improvement with a descriptive message
8. Regenerates `bootstrap/cli.c` if any compiler module was modified
9. Pushes the branch and opens a PR
10. Exits with code 42, which tells the supervisor to rebuild and start the next cycle

The supervisor (`./a agent`) manages the lifecycle: it builds the agent binary, runs it, and on exit 42, rebuilds from source (picking up any changes the agent made to its own dependencies) and loops. Crashes get exponential backoff restarts.

---

## Hardware Requirements

The agent compiles C code and runs test suites. It is not compute-intensive.

**Minimum:**
- 2 CPU cores
- 4 GB RAM
- 10 GB disk
- Persistent network connection (for git push and PR creation)

**Recommended:**
- 4 CPU cores (compilation is the bottleneck; more cores help `gcc`)
- 8 GB RAM
- SSD (faster I/O for test execution)

**What works:**
- A $5-10/month Linux VPS (Hetzner, DigitalOcean, Linode)
- A Mac Mini or old laptop that stays on
- A Raspberry Pi 4/5 (slower, but functional)
- A GitHub Codespace (if you want cloud + editor access)

The agent produces native binaries with zero external dependencies. There is no GPU requirement, no container runtime, no database server. Just `gcc`, `git`, and a shell.

---

## Software Prerequisites

Install these on the host machine:

| Tool | Purpose | Install |
|------|---------|---------|
| `gcc` | Compiles generated C code | `apt install gcc` / `xcode-select --install` |
| `git` | Version control | `apt install git` / pre-installed on macOS |
| `gh` | GitHub CLI for creating PRs | `apt install gh` / `brew install gh` |
| `timeout` | Limits test execution time | Included in coreutils (Linux); `brew install coreutils` for `gtimeout` on macOS |

---

## Step 1: Clone and Build

```bash
git clone git@github.com:vesche/a.git
cd a
./build.sh
./a --version   # should print "a 2.1.x"
```

If `build.sh` succeeds, you have a working `./a` binary. Everything else builds from this.

---

## Step 2: Set Up Git Credentials

The agent pushes branches and creates PRs. It needs write access to the repository.

### Option A: SSH key (recommended for a dedicated machine)

```bash
# Generate a key for the agent (no passphrase)
ssh-keygen -t ed25519 -f ~/.ssh/id_a_agent -N ""

# Add to ssh config
cat >> ~/.ssh/config << 'EOF'
Host github.com
  IdentityFile ~/.ssh/id_a_agent
  IdentitiesOnly yes
EOF

# Add the public key to GitHub:
#   Settings > SSH and GPG keys > New SSH key
cat ~/.ssh/id_a_agent.pub
```

### Option B: GitHub CLI auth (simpler, interactive one-time setup)

```bash
gh auth login
# Follow the prompts: GitHub.com > SSH > authenticate via browser
# This also configures git credential storage
```

### Option C: Personal access token (for headless/CI environments)

```bash
# Create a fine-grained PAT at: github.com/settings/tokens
# Scopes needed: contents (read/write), pull_requests (read/write)
echo "https://YOUR_TOKEN@github.com" > ~/.git-credentials
git config --global credential.helper store
gh auth login --with-token < /path/to/token.txt
```

### Verify

```bash
cd /path/to/a
git push --dry-run origin main   # should succeed (nothing to push)
gh auth status                    # should show logged in
```

---

## Step 3: Configure Git Identity

The agent's commits need an author. Set this in the repo (not globally, to avoid affecting your personal commits on the same machine):

```bash
cd /path/to/a
git config user.name "a-self-improver"
git config user.email "a-self-improver@noreply.github.com"
```

---

## Step 4: Test a Single Cycle

Before setting up a service, run one cycle manually to confirm everything works:

```bash
cd /path/to/a
./a agent agents/self_improve.a --name self-improver --no-restart 2>&1 | tee /tmp/self-improve-test.log
```

The `--no-restart` flag makes the supervisor stop after the first cycle instead of looping. Watch the output for:

- `registered` -- agent started
- `created branch: self-improve/run-1` -- git is working
- `IMPROVED!` -- a validated optimization was applied (may not happen every run)
- `pushed to origin/...` -- git push succeeded
- `PR opened: https://...` -- PR was created

If the cycle completes without errors, you're ready for continuous operation.

Clean up after the test:

```bash
git checkout main
git branch -D self-improve/run-1 2>/dev/null
```

---

## Step 5: Set Up a Service Runner

The agent should survive reboots and terminal closures. Choose one:

### Linux: systemd (recommended)

Create `/etc/systemd/system/a-self-improve.service`:

```ini
[Unit]
Description=a-lang self-improvement agent
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=YOUR_USERNAME
WorkingDirectory=/path/to/a
ExecStart=/path/to/a/a agent agents/self_improve.a --name self-improver --max-restarts 0
Restart=on-failure
RestartSec=60
StandardOutput=append:/var/log/a-self-improve.log
StandardError=append:/var/log/a-self-improve.log
Environment=HOME=/home/YOUR_USERNAME
Environment=PATH=/usr/local/bin:/usr/bin:/bin

[Install]
WantedBy=multi-user.target
```

Then:

```bash
sudo systemctl daemon-reload
sudo systemctl enable a-self-improve
sudo systemctl start a-self-improve

# Check status
sudo systemctl status a-self-improve
journalctl -u a-self-improve -f   # follow logs
```

### macOS: launchd

Create `~/Library/LaunchAgents/com.a-lang.self-improve.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
  "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>com.a-lang.self-improve</string>
  <key>ProgramArguments</key>
  <array>
    <string>/path/to/a/a</string>
    <string>agent</string>
    <string>agents/self_improve.a</string>
    <string>--name</string>
    <string>self-improver</string>
    <string>--max-restarts</string>
    <string>0</string>
  </array>
  <key>WorkingDirectory</key>
  <string>/path/to/a</string>
  <key>RunAtLoad</key>
  <true/>
  <key>KeepAlive</key>
  <true/>
  <key>StandardOutPath</key>
  <string>/tmp/a-self-improve.log</string>
  <key>StandardErrorPath</key>
  <string>/tmp/a-self-improve.log</string>
</dict>
</plist>
```

Then:

```bash
launchctl load ~/Library/LaunchAgents/com.a-lang.self-improve.plist

# Check status
launchctl list | grep a-lang
tail -f /tmp/a-self-improve.log
```

### Quick and dirty: tmux/screen

If you just want to try it without a service manager:

```bash
tmux new -s self-improve
cd /path/to/a
./a agent agents/self_improve.a --name self-improver --max-restarts 0 2>&1 | tee -a /tmp/self-improve.log
# Ctrl-B D to detach; tmux attach -t self-improve to reattach
```

---

## Step 6: Enable Auto-Merge (Optional)

The repo includes `.github/workflows/auto-merge.yml` which automatically squash-merges PRs from `self-improve/*` branches once CI passes.

For this to work, enable these GitHub repo settings:

1. **Settings > General > Pull Requests**: check "Allow auto-merge"
2. **Settings > Branches > Branch protection rules** for `main`:
   - Require status checks to pass (select the "Build & Test" workflow)
   - This ensures the auto-merge only fires after CI is green

Without auto-merge enabled, PRs will be created but require manual merge. This is the safer starting point.

---

## Monitoring

| What | How |
|------|-----|
| Live logs | `tail -f /tmp/a-self-improve.log` (or wherever you redirect stderr) |
| Agent state | `sqlite3 ~/.a/agent_state.db "SELECT value FROM kv WHERE key='agent:self-improver:state'"` |
| Open PRs | `gh pr list --head self-improve/ --state open` |
| Service status | `systemctl status a-self-improve` / `launchctl list \| grep a-lang` |
| Registry | `cat /tmp/a_agents/self-improver.json` (while running) |

---

## Stopping the Agent

```bash
# systemd
sudo systemctl stop a-self-improve

# launchd
launchctl unload ~/Library/LaunchAgents/com.a-lang.self-improve.plist

# tmux
tmux kill-session -t self-improve

# Any method: the supervisor writes its PID
kill $(cat /tmp/a_agents/self-improver.pid)
```

After stopping, the agent's state persists in `~/.a/agent_state.db`. The next time it starts, it picks up where it left off (run count, lifetime stats).

---

## Troubleshooting

**"agent: build failed"** -- The agent source doesn't compile. Run `./a build agents/self_improve.a -o /tmp/test` manually to see the error.

**"WARNING: push failed"** -- Git credentials aren't configured. Run `git push --dry-run origin main` to test.

**"WARNING: PR creation failed"** -- `gh` isn't authenticated. Run `gh auth status`.

**"WARNING: could not create branch"** -- The branch already exists from a previous interrupted run. Delete it: `git branch -D self-improve/run-N`.

**Agent makes no improvements** -- This is normal. The optimizer only applies high-priority inlining of hot functions. Many modules have no applicable transforms. The agent will still generate tests for untested modules.

**Agent keeps restarting quickly** -- Check if a cycle is crashing (exit code != 0 and != 42). The supervisor logs crash details. Common cause: a test depends on network access that isn't available.

---

## Summary

```
Hardware:    Any machine with 2+ cores, 4GB RAM, and a network connection
Software:    gcc, git, gh CLI, coreutils (timeout)
Credentials: SSH key or gh auth login, plus repo-local git identity
Service:     systemd (Linux), launchd (macOS), or tmux
Monitor:     Log file + sqlite3 state DB + gh pr list
```

The total setup time is about 15 minutes. Once running, the agent operates indefinitely: profile, optimize, test, commit, push, PR, rebuild, repeat. CI validates every PR. Auto-merge is optional.
