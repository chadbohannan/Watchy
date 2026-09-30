# Engineering Wiki — Schema & Operating Rules

This is a persistent, LLM-maintained knowledge base for capturing complex system behavior, incident patterns, debugging knowledge, and operational wisdom about a codebase. The LLM writes and maintains all wiki content. The human curates sources, directs investigation, and asks questions.

This wiki documents **Watchy**, an open-source ESP32 e-paper watch firmware library by SQFMI. The wiki's own scripts live at the repo root under `scripts/` (see [scripts/README.md](../scripts/README.md)); this operating contract sits at `wiki/CLAUDE.md`. The directory layout, page conventions, and operations below are the standing contract for maintaining it.

## Directory Layout

```
.
├── repo root
│   ├── CLAUDE.md / scripts/   # Scripts from the code-wiki-template
│   └── repos.txt              # Machine-local repo paths (gitignored)
└── wiki/                      # LLM-generated and LLM-maintained markdown pages
    ├── CLAUDE.md              # This file — schema and operating rules
    ├── index.md               # Content catalog — the structured routing layer
    ├── log.md                 # Chronological record of all operations
    ├── systems/               # Distinct systems, services, and deployments
    ├── incidents/             # Specific incidents, outages, and postmortems
    ├── components/            # Individual components, libraries, and dependencies
    ├── concepts/              # Recurring patterns, failure modes, and architectural concepts
    ├── runbooks/              # Operational procedures synthesized from incident history
    └── syntheses/             # Cross-cutting analyses, comparisons, and thematic summaries
sources/                       # Raw, immutable source documents (never modified)
```

`sources/` holds inputs verbatim — design notes, datasheets, log dumps. The LLM reads them but never edits them. Everything under `wiki/` is LLM-authored and LLM-maintained.

## Page Conventions

Wiki pages are prose documents. They have no YAML frontmatter. The directory a page lives in determines its category. The first heading serves as the title.

Internal links use relative paths and are embedded in context — every link should appear inside a sentence that explains why the relationship matters. A link to another page is a claim about a connection, and the surrounding prose is the evidence.

```markdown
# Page Title

Opening paragraph establishing what this page covers and why it matters.

Body paragraphs with contextual links woven in. For example: "The menu is
driven by `guiState` state that survives deep sleep, and screen rendering
funnels through the graphics driver."
```

Source attribution is inline. When a factual claim traces to a specific source, cite it naturally: "the `config.h` v1.4.14 build describes..." This gives citations semantic context — the reader understands not just *what* the source is, but *what claim it supports and why it was consulted.*

### Naming Conventions

- **Systems**: `{system-name}.md`
- **Incidents**: `{YYYY-MM-DD}-{short-description}.md`
- **Components**: `{component-name}.md`
- **Concepts**: `{concept-name}.md`
- **Runbooks**: `{procedure-name}.md`
- **Syntheses**: `{analysis-topic}.md`

Filenames are lowercase, hyphenated, and descriptive.

### Writing Style

Every sentence should earn its place. A short, precise page is better than a long, vague one. Write in plain, humanist prose — not corporate-memo bullet lists.

For incidents, open with a plain-English summary that captures severity, duration, affected systems, and resolution in a sentence or two. Cross-references are a first-class feature. When creating or updating a page, consider what other pages should link here, and what this page should link to. But every link must be contextual — no "see also" lists at the bottom.

Every concrete claim — a field name, a file path, a GPIO pin, an I2C address — is implicitly anchored either to current code (in which case sync must keep it true) or to a named source document. Before extending or relying on a code-anchored citation in an existing page, verify it still resolves.

When new information contradicts existing wiki content, don't silently overwrite. Note the contradiction explicitly, cite both sources, and flag it for the user. If a claim comes from a single source or is speculative, say so.

### Density and Single Ownership

Maximize information per token — *information*, not text. Cut any token the reader can already see and that can't differ from it. Keep anything the reader can't reconstruct from what's in front of them, even if another layer also holds it.

Git records *edit*-time, invisible to anyone reading rendered markdown. So event dates live in the prose, and when a sync confirms a page's code-anchored claims, record verify-time: "synced against `Watchy`@`a1b2c3d` as of 2026-06."

Links stay inline because that's denser. Duplicate *immutable* facts freely. Give every *mutable* fact — a config value, a threshold, a pin — one owning page and link to it.

## The Index

`wiki/index.md` is the structured routing layer. It is the first thing an LLM reads when answering a query. Each entry is a link with a one-line description, grouped by category. The index links only to wiki pages; do not clutter it with links to raw `sources/` material. It is updated on every ingest.

## The Log

`wiki/log.md` is a chronological, append-only record of wiki operations. Each entry is a single line with a consistent prefix for parseability:

```
[YYYY-MM-DD] operation | Subject — brief description of what happened
```

Operations: `ingest`, `query`, `lint`, `enrich`, `sync`, `update`.

## Discovery Layers

This wiki is fundamentally *about* a codebase, so wiki pages should describe code behavior, fields, surfaces, and configuration directly. The wiki is the narrative and historical layer over the code; the code is the ground truth.

- **The wiki** is the narrative layer: how components fit together, what their fields mean, how they fail, how they're operated, what changed and why and when. Be dense with concrete detail: dates, commit ranges, configuration values, file paths, function names, thresholds, GPIO pins, I2C addresses.
- **The code** is ground truth. Reach into it via `grep`/`glob`/`read` over the checkout listed in `repos.txt` (here: `/home/kali/workspace/Watchy`). The relevant source lives under `src/`.
- **`index.md`** is the routing layer. An LLM reads the index to find relevant wiki pages, then drills into source code when the wiki points toward a specific component or behavior.

**Bias toward ingesting.** If a change, source, or commit relates to Watchy, default to capturing it somewhere. The cost of a small, accurate update is low; the cost of a missing fact during debugging is high.

## Operations

### Ingest

When the user provides a new source (a design note, debug log, code snippet, commit history, etc.):

1. **Read** the source fully.
2. **Create or update** wiki pages:
   - Use Simplified Technical English.
   - Create or update entity pages for every system, component, or concept mentioned.
   - Create or update incident pages if the source describes an incident.
   - Extract any operational procedures into runbook pages.
   - Weave cross-references into the prose of every page touched.
   - Split larger files on topical boundaries to optimize lookup via index.md.
3. **Update index.md** — add or revise entries for every wiki page touched.
4. **Append to log.md** — a temporal index, no more than 400 characters recording the ingest.
5. **Report** to the user: what pages were created/updated, what connections were found, what gaps remain.

### Query

When the user asks a question:

1. **Read index.md** first to identify relevant pages.
2. **Read** the relevant wiki pages.
3. **Use code search** (grep/glob/read over the local checkout) if the question needs grounding in source code.
4. **Synthesize** an answer with citations to wiki pages and original sources.
5. If the answer is substantial and reusable, **offer to file it** as a new page.
6. **Log** the query.

When the user asks for help reviewing a merge/pull request:

1. **Verify the repos are up to date** — run the sync pull.
2. **Pull the default branch.**
3. **Ingest the diff.** The state of the system has changed, so pull that knowledge into the wiki, then lint and refine.
4. **Support the user** — be interactive and let the user steer.

### Lint

When the user asks for a health check:

1. Check for **contradictions** between pages.
2. Find **orphan pages** — pages not linked from any other page (`index.md` links don't count).
3. Identify **mentioned-but-missing** entities that deserve their own page.
4. Flag **stale pages** — prefer a page's recorded verify-time stamp where it has one.
5. Verify **link integrity** — all internal links resolve to existing files.
6. Suggest **new questions** or sources that would fill gaps.
7. Report findings and fix what can be fixed automatically.
8. **Log** the lint pass.

The structural checks are automated by `scripts/lint-wiki.py`. Run it first, then handle the prose-level checks by reading.

### Enrich

When deeper context could improve clarity:

1. Search the local checkout for relevant source code, architecture patterns, and implementation details.
2. Integrate findings into wiki pages as prose with citations to specific code paths.
3. **Log** the enrichment.

### Sync

When the user asks to sync the wiki against recent code changes:

1. Run `scripts/pull-repos.sh`. It reads `repos.txt` and fast-forwards `main`/`master` for each tracked repo.
2. For each repo that moved, inspect the new commits and their diffs for Watchy-relevant changes — new fields, behavior changes, config knobs, pin changes. Default to capturing rather than skipping.
3. **Hunt for broken anchors.** For each diff, grep the wiki for the field names, file paths, function names, or GPIO numbers it touches.
4. **Discuss** with the user which changes are wiki-relevant before writing anything.
5. **Update** affected wiki pages, weaving cross-references and citing specific commits or files. Record verify-time in the prose when claims are confirmed against HEAD.
6. **Append to log.md** — one entry per sync, noting the commit range(s) and which pages were touched.

## Incident Page Structure

Incident pages don't use a rigid template, but should cover these aspects in whatever order makes the narrative clearest:

- **Opening summary**: severity, duration, affected systems, and outcome in one or two sentences.
- **Timeline**: what happened and when.
- **Root cause**: what actually broke and why.
- **Detection**: how it was noticed.
- **Resolution**: what fixed it.
- **Contributing factors**: systemic issues that made this incident possible, with links to relevant concept and component pages.
- **Patterns**: connections to other incidents and recurring failure modes, woven into the narrative.
