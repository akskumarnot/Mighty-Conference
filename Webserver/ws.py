#!/usr/bin/python
import json
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
import urlparse,cgi,os,Queue,sys
import pyio		#module for talking to the C server socket
#custom user made module
import router,session
#commons
import commons
#concurrecy assistance is good
from threading import Thread


fifo_path = "./../probe.fifo"

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
		router.post(self,path,post_data)	#invoking the webapi and posting to the server
		return

def init_web_server():
	try:
		print 'thread started'
    		server = HTTPServer(('localhost', 8080), webHandler)
    		print 'HTTPServer started'
		os.chdir('./../Webserver/')
    		server.serve_forever()
	except KeyboardInterrupt:
    		print 'server.socket.close()'
    		server.socket.close()

def command_parser(line):
	data = json.loads(line)
	if 'AUTH' in data.keys() and data['AUTH']=='GRANTED':
		commons.got_auth_message = True
	if 'CLIENTS_LIST' in data.keys() and data['CLIENTS_LIST']!='(null)':
		print 'will add client to list,sooon'

def init_fifo_client():
	print 'fifo client started'
	global fifo_path
	fifo = open(fifo_path, "r")
	for line in fifo:
    		command_parser(line)
	fifo.close()

def main():
	#init the webserver
	thread  = Thread(target=init_web_server)
	thread.start()
	#init the fifo client
	thread = Thread(target=init_fifo_client)
	thread.start()
	while True:
		pass
