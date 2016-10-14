FROM thoughtram/keras

RUN pip install gunicorn json-logging-py

RUN mkdir -p /opt/signtalker

COPY logging.conf /opt/signtalker/logging.conf
COPY gunicorn.conf /opt/signtalker/gunicorn.conf

COPY signtalkerServer.py /opt/signtalker/signtalkerServer.py

ENTRYPOINT ["/usr/local/bin/gunicorn", "--config", "/opt/signtalker/gunicorn.conf", "--log-config", "/opt/signtalker/logging.conf", "-b", "0.0.0.0:8000", "--chdir", "/opt/signtalker", "signtalkerServer:app", "&"]
#ENTRYPOINT ["/usr/local/bin/gunicorn", "-b", "0.0.0.0:8000", "myapp:app"]
