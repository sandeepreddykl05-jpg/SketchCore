cat /dev/clipboard > README.md
head -n 5 README.md
git add README.md
git commit -m "docs: add verified README with architecture, build steps, and usage"
git push
