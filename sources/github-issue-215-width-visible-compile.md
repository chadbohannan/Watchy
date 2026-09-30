# GitHub Issue #215 — "'WIDTH_VISIBLE' is not a member of 'WatchyDisplay'"

Source: https://github.com/sqfmi/Watchy/issues/215 (closed 2023-05-01, 11 comments, created 2023-04-18).

## Summary

Compiling watch faces in Arduino 2 IDE fails with:

```
/home/.../GxEPD2/src/GxEPD2_BW.h: In instantiation of 'GxEPD2_BW<GxEPD2_Type, page_height>::GxEPD2_BW(GxEPD2_Type) [with GxEPD2_Type = WatchyDisplay; ...]':
.../Watchy/src/Watchy.cpp:5:69:   required from here
.../GxEPD2_BW.h:208:134: error: 'WIDTH_VISIBLE' is not a member of 'WatchyDisplay'
```

User selected "Watchy" board, partition scheme "huge", installed all libraries; error occurs on all boards.

## Comment thread highlights

- Root cause: **GxEPD2 library version 1.5.2** introduced a `WIDTH_VISIBLE` variable. As a workaround, use **GxEPD2 1.5.1**.
- Commenter identified the Watchy `Display.h` is **highly coupled to GxEPD2** and needs maintenance whenever anything changes in the base library: "They added a static member that is not reflected in Display.h."
- `library.json`/`dependencies` reference pointing at GxEPD2 1.5.1 needed updating.
- Maintainer pushed a fix in **Watchy v1.4.7**; confirmed compiling in the Arduino editor afterward.

## Relevance

Demonstrates the tight coupling between the Watchy Display subsystem and the upstream GxEPD2 library (ZinggJM). Upstream library changes that add to the `GxEPD2_EPD` interface can break the vendored `WatchyDisplay` class until `Display.h` is updated. This is the kind of dependency drift the wiki's Sync operation should watch for.
