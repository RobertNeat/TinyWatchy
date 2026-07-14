"""Apply narrowly scoped compatibility fixes to pinned PlatformIO libraries."""

from pathlib import Path

Import("env")  # type: ignore[name-defined]  # Provided by PlatformIO/SCons.


def replace_once(path: Path, old: str, new: str) -> None:
    content = path.read_text(encoding="utf-8")
    if new in content:
        return
    if content.count(old) != 1:
        raise RuntimeError(f"Expected exactly one occurrence of {old!r} in {path}")
    path.write_text(content.replace(old, new), encoding="utf-8")


libdeps_dir = Path(env.subst("$PROJECT_LIBDEPS_DIR")) / env.subst("$PIOENV")
rtc_candidates = [
    path
    for path in libdeps_dir.glob("Rtc_Pcf8563*/src")
    if (path / "Rtc_Pcf8563.h").is_file()
]

if len(rtc_candidates) != 1:
    raise RuntimeError(
        "Expected one installed Rtc_Pcf8563 library, found "
        f"{len(rtc_candidates)} in {libdeps_dir}"
    )

rtc_source = rtc_candidates[0]
header = rtc_source / "Rtc_Pcf8563.h"
implementation = rtc_source / "Rtc_Pcf8563.cpp"

if '#define RTCC_VERSION  "Pcf8563 v1.0.3"' not in header.read_text(encoding="utf-8"):
    raise RuntimeError("Rtc_Pcf8563 compatibility patch supports version 1.0.3 only")

# Upstream 1.0.3 returns a string literal through char*. Make the API const-correct
# instead of suppressing the compiler diagnostic.
replace_once(header, "        char *version();", "        const char *version();")
replace_once(
    implementation,
    "char *Rtc_Pcf8563::version(){",
    "const char *Rtc_Pcf8563::version(){",
)
