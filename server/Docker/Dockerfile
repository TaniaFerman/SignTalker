FROM thoughtram/keras

RUN pip install gunicorn json-logging-py requests tqdm falcon simplejson

RUN mkdir -p /opt/signtalkertrainer

COPY logging.conf /opt/signtalkertrainer/logging.conf
COPY gunicorn.conf /opt/signtalkertrainer/gunicorn.conf

#ENTRYPOINT ["/usr/local/bin/gunicorn", "--config", "/opt/signtalkertrainer/gunicorn.conf", "--log-config", "/opt/signtalkertrainer/logging.conf", "-b", "0.0.0.0:8000", "--chdir", "/projects/signtalkertrainer/tensorflow-csv-to-api", "irisweb:api", "&"]
#ENTRYPOINT ["/usr/local/bin/gunicorn", "-b", "0.0.0.0:8000", "myapp:app"]
