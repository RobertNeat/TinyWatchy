# Ideas and future work

This file contains proposals, not implemented features.

## Automatic light and dark watch-face variants

Allow a watch face to provide light and dark variants and select one according to a configurable daytime interval.

Possible configuration:

- start hour for light mode;
- end hour for light mode;
- naming or metadata that links both variants;
- a fallback when only one variant exists.

The design should avoid duplicating watch-face selection entries and should define how the theme interacts with menu rendering and partial e-paper refreshes.
