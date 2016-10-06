import logging
import json

logger = logging.getLogger(__name__)

def app(environ, start_response):
    try:
        request_body_size = int(environ.get('CONTENT_LENGTH', 0))
    except (ValueError):
        request_body_size = 0

    request_body = environ['wsgi.input'].read(request_body_size)
    inputdata = json.loads(request_body)

    logger.info(inputdata, extra={'tags': ['role:web', 'env:prod']})
	
    outputdata = "abcd"
    logger.info(outputdata)
    start_response("200 OK", [
          ("Content-Type", "text/plain"),
          ("Content-Length", str(len(outputdata)))
      ])
    return iter([outputdata])