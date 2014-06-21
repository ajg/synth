
echo '{"user": "Dolph Lundgren"}' > 'context.json'
echo 'Howdy, {{ user }}!' > 'template.txt'

cat template.txt | ./synth -e django -c context.json
