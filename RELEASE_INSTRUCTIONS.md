# Instructions for Creating GitHub Release 6.0 - "stable"

This directory contains everything needed to create the 6.0 "stable" release of the Adaptive Radix Tree library.

## Files Created

1. **CHANGELOG.md** - Complete changelog following Keep a Changelog format
2. **RELEASE_NOTES.md** - Detailed release notes for GitHub release
3. **create_release.sh** - Automated script to create the release
4. **Tag 6.0** - Git annotated tag (already created locally)

## What Has Been Prepared

✅ **Git Tag Created**: Annotated tag `6.0` with full release message  
✅ **Changelog Written**: Comprehensive changelog in CHANGELOG.md  
✅ **Release Notes**: Detailed release notes in RELEASE_NOTES.md  
✅ **Contributors Acknowledged**: All commit authors, issue reporters, and PR authors mentioned  

## To Complete the Release

Since I (copilot-swe-agent) cannot push tags or create GitHub releases directly, you need to:

### Option 1: Manual GitHub Release (Web UI)

1. **Push the tag**:
   ```bash
   git push origin 6.0
   ```

2. **Create release on GitHub**:
   - Go to https://github.com/rafaelkallis/adaptive-radix-tree/releases/new
   - Select tag: `6.0`
   - Release title: `6.0 - stable`
   - Copy contents from `RELEASE_NOTES.md` into the description
   - Click "Publish release"

### Option 2: Automated Script (Recommended)

Run the provided script (requires GitHub CLI authentication):

```bash
./create_release.sh
```

This will:
1. Push the tag to GitHub
2. Create the release with the name "stable"
3. Attach the release notes from RELEASE_NOTES.md

### Option 3: GitHub CLI Manual

```bash
# Push tag
git push origin 6.0

# Create release
gh release create 6.0 \
    --title "6.0 - stable" \
    --notes-file RELEASE_NOTES.md \
    --repo rafaelkallis/adaptive-radix-tree
```

## Verification

After creating the release, verify:

- ✅ Tag 6.0 is visible at https://github.com/rafaelkallis/adaptive-radix-tree/tags
- ✅ Release is visible at https://github.com/rafaelkallis/adaptive-radix-tree/releases
- ✅ Release name is "6.0 - stable"
- ✅ Release notes display correctly with all acknowledgments
- ✅ CHANGELOG.md is committed to the repository

## Acknowledgments in Release

The release properly acknowledges:

**Issue Reporters:**
- @hiqsociety (Issue #16 - memory benchmarking request)
- @Haibarapink (Issue #21 - buffer overflow bug)
- @YelaShady (Issue #19 - iterator crash)

**Code Contributors:**
- @rafaelkallis (maintainer, CI/CD, build fixes)
- @bowling233 (co-author of node growth fix)
- @Copilot (bug fixes and features)
- @copilot-swe-agent[bot] (automated contributions and release preparation)

**Myself:**
- @copilot-swe-agent[bot] (release preparation, changelog creation)

## Contents of Tag 6.0

The annotated tag includes:
- Release title: "Release 6.0 - stable"
- Summary of key changes
- List of all contributors
- Reference to full changelog

View with:
```bash
git show 6.0
```
