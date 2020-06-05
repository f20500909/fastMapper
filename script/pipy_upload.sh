cd ..

python3 setup.py check
python3 setup.py sdist build



python3 setup.py sdist upload

twine upload dist/*
