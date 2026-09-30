# Compile Break on GxEPD2 1.5.2 (`WIDTH_VISIBLE`)

A dependency-driven compile failure when building against an upgraded GxEPD2 library, from [GitHub issue #215](https://github.com/sqfmi/Watchy/issues/215) (closed 2023-05-01, 11 comments).

Upgrading to **GxEPD2 1.5.2** broke every build with a hard template error: `'WIDTH_VISIBLE' is not a member of 'WatchyDisplay'` instantiating `GxEPD2_BW<WatchyDisplay, ...>`. GxEPD2 1.5.2 introduced a new `WIDTH_VISIBLE` static member to the panel interface, but the vendored `WatchyDisplay` in this repo's `Display.h` did not define it, so the [Display](../components/display.md) class failed to satisfy the base template. The upstream library change therefore surfaced as a Watchy compile error even though no Watchy source had changed.

Two workarounds existed: pin **GxEPD2 to 1.5.1** (the last version without the new member), or wait for Watchy to add the member. The maintainer pushed a fix in **Watchy v1.4.7** that added `WIDTH_VISIBLE` to `Display.h`, after which builds compiled cleanly in the Arduino editor. The thread also noted the Watchy `Display.h` is "highly coupled" to GxEPD2 and needs maintenance whenever the base library changes.

The durable lesson for the [Sync](../systems/watchy-firmware.md) operation is dependency drift on the GxEPD2 interface: any upstream addition to `GxEPD2_EPD`'s static surface should be checked against `WatchyDisplay` when GxEPD2's version moves. This is flagged on the [Display](../components/display.md) page as the reason the vendored driver is updated in lockstep with GxEPD2.
