# 1. Base image
FROM python:3.8.3-slim-buster

# 2. Copy files
WORKDIR /project

COPY requirements.txt ./
RUN pip install -r requirements.txt

# 3. Install dependencies
COPY . .

ENTRYPOINT [ "python", "main.py" ]