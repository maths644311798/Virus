# API for laugh detection

[Minkai Xu](https://github.com/MinkaiXu)

## Installation

- python 3.6 recommended, with pip installed

```bash
pip install baidu-aip
python setup.py install
```

## Code description

- `laugh_detection.py` is the main code for using the api. 

## Run

- run

```python
from face_apk.laugh_detection import laugh_detection
whether_laugh = laugh_detection('figure/***.jpg')
# this will return a bool to indicate whether laugh
```
