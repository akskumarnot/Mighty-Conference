#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
import urlparse,cgi,os,Queue
import pyio		#module for talking to the C server socket
#custom user made module
import router,session
#commons
import commons
#concurrecy assistance is good
from threading import Thread
from time import sleep

class webHandler(BaseHTTPRequestHandler):
	
	def do_GET(self):
		router.init(self)
		path = urlparse.urlparse(self.path).path
		#this for fetching a file
		if router.getFile(path):	#if true means was requesting a file
			return
		#this is for fetching a view
		router.get(path)
		return
	
	def do_POST(self):
		length = int(self.headers['Content-Length'])
		post_data = urlparse.parse_qs(self.rfile.read(length).decode('utf-8'))
		path = urlparse.urlparse(self.path).path
		router.post(path,post_data)	#invoking the webapi and posting to the server
		return

def init_server():
	try:
		print 'thread started'
    		server = HTTPServer(('localhost', 8080), webHandler)
    		print 'HTTPServer started'
		os.chdir('./../Webserver/')
    		server.serve_forever()
	except KeyboardInterrupt:
    		print 'server.socket.close()'
    		server.socket.close()


def main():
	#init the webserver
	thread  = Thread(target=init_server)
	thread.start()
	while True:
		pass
	

def auth_state(status):
	print 'uhhh uhhhh uhhh , ahhhh ahh ahhh'
	if status=='access':
		#need to start session
		session.start()
		session.session['logged']=True
		#need to queue this for the webclient to get updated
	commons.mq.put({"type":"auth","status":+status})	
	commons.got_auth_message = True
