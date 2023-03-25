while inotifywait \
    -r /home/jfa/projects/ai/src/aiviz/frontend/* --exclude 'node_modules/'; do
    rsync -rtuvi --delete --exclude 'node_modules/' \
    ./src/aiviz/frontend/ \
    rsync://192.168.1.162/mabServer
done
