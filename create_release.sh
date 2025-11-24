#!/bin/bash
# Script to create GitHub Release 6.0 - "stable"
# 
# This script should be run by a user with GitHub authentication
# Run: bash create_release.sh

set -e

echo "Creating GitHub Release 6.0 - 'stable'"
echo "========================================"
echo ""

# Check if gh CLI is available
if ! command -v gh &> /dev/null; then
    echo "Error: GitHub CLI (gh) is not installed"
    echo "Install it from: https://cli.github.com/"
    exit 1
fi

# Check if authenticated
if ! gh auth status &> /dev/null; then
    echo "Error: Not authenticated with GitHub"
    echo "Run: gh auth login"
    exit 1
fi

# Push the tag
echo "Pushing tag 6.0..."
git push origin 6.0

# Create the release
echo "Creating GitHub release..."
gh release create 6.0 \
    --title "6.0 - stable" \
    --notes-file RELEASE_NOTES.md \
    --repo rafaelkallis/adaptive-radix-tree

echo ""
echo "✅ Release 6.0 created successfully!"
echo "View it at: https://github.com/rafaelkallis/adaptive-radix-tree/releases/tag/6.0"
