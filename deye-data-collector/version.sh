#!/bin/bash

# Read version from VERSION file
VERSION=$(cat VERSION)

# Validate that VERSION is not empty
if [ -z "$VERSION" ]; then
    echo "Error: VERSION file is empty or missing!"
    exit 1
fi

# Update version in config.json
if [ -f "config.json" ]; then
    jq --arg ver "$VERSION" '.version = $ver' config.json > config.json.tmp && mv config.json.tmp config.json
    echo "Updated config.json to version $VERSION"
fi

# Update version in config.yaml
if [ -f "config.yaml" ]; then
    # Update version field
    sed -i "s/^version: .*/version: \"$VERSION\"/" config.yaml
    
    echo "Updated config.yaml to version $VERSION"
fi

echo "Version update complete!"

