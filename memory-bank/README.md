# Memory Bank README

**Purpose:** Persistent context storage for PICO_OTA development sessions

---

## File Structure

| File | Purpose |
|------|---------|
| `projectbrief.md` | Goals, constraints, tech stack, release management |
| `activeContext.md` | Current focus, recent changes, blockers, next actions |
| `SESSION.md` | Append-only log of all work sessions (YYYY-MM-DD — vX.Y.Z) |
| `master-plan.md` | Milestones, version history, roadmap |
| `README.md` | This file — usage guide for memory bank |

---

## Read Order (Start of Session)

1. `projectbrief.md` — Understand project scope and constraints
2. `activeContext.md` — Understand current state and blockers
3. `SESSION.md` — Review recent work history
4. `master-plan.md` — Understand milestones and roadmap

---

## Update Protocol

- **SESSION.md**: Append new entry at top for each work session
- **activeContext.md**: Update "Current Focus" and "Blockers" as work progresses
- **master-plan.md**: Update milestones when multi-step work is scoped
- **projectbrief.md**: Rarely updated (only for major project changes)

---

## Maintenance

- Keep entries concise (session logs ≤ 20 lines each)
- Use semantic versioning in session logs
- Mark blockers as resolved when cleared
- Archive old sessions if file exceeds 500 lines
