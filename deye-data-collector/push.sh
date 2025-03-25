#!/bin/bash

# --- CONFIGURATION ---
GITHUB_USER="pr4u4t"  # Replace with your GitHub username
IMAGE_NAME="deye-data-collector"    # Replace with your Docker image name
VERSION=$(cat VERSION)              # Change version as needed
GITHUB_PAT=$(cat PAT)  # Replace with your GitHub PAT

./version.sh

# --- LOGIN TO GHCR ---
echo "🔑 Logging into GitHub Container Registry..."
echo "$GITHUB_PAT" | docker login ghcr.io -u "$GITHUB_USER" --password-stdin

# --- BUILD & TAG IMAGE ---
echo "🐳 Building Docker image..."
docker build -t ghcr.io/$GITHUB_USER/$IMAGE_NAME:$VERSION .

echo "🏷️ Tagging latest version..."
docker tag ghcr.io/$GITHUB_USER/$IMAGE_NAME:$VERSION ghcr.io/$GITHUB_USER/$IMAGE_NAME:latest

# --- PUSH IMAGE TO GHCR ---
echo "📤 Pushing versioned image to GHCR..."
docker push ghcr.io/$GITHUB_USER/$IMAGE_NAME:$VERSION

echo "📤 Pushing latest tag to GHCR..."
docker push ghcr.io/$GITHUB_USER/$IMAGE_NAME:latest

echo "✅ Done! Your image is available at:"
echo "   ghcr.io/$GITHUB_USER/$IMAGE_NAME:$VERSION"
echo "   ghcr.io/$GITHUB_USER/$IMAGE_NAME:latest"
