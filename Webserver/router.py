#need to extend the interpreter for login using the extension
import pyio,session,commons,pyjson
#global definitions 
	#instance of the handler
handler_obj = None

def get(path):
	#route for the landing page
	if path is '/':
		f=open('./views/land.html')
		handler_obj.send_response(200)
		handler_obj.send_header('Content-type','text/html')
		handler_obj.end_headers()
		handler_obj.wfile.write(f.read())	
		return
	if path=='/gather':
		if session.session['logged']:
			f=open('./views/gather.html')
			handler_obj.send_response(200)
			handler_obj.send_header('Content-type','text/html')
			handler_obj.end_headers()
			handler_obj.wfile.write(f.read())
		elif path=='/logerr':
			f=open('./views/logerr.html')
			handler_obj.send_response(200)
			handler_obj.send_header('Content-type','text/html')
			handler_obj.end_headers()
			handler_obj.wfile.write(f.read())
	#this is for sending login values to the server
	
		
def getFile(name):
	loc=''
	if name.endswith(".js"):
		loc='./scripts/'+name
	if name.endswith(".css"):
		loc='./style/'+name
	if loc is '':	#this means that hasn't requested a file
		return False

	try:
		f=open(loc)	#need to handle IOError here for a file not present
	except IOError, e:
		print e.errno
		print e
		return
	handler_obj.send_response(200)
	handler_obj.send_header('Content-type','text/html')
	handler_obj.end_headers()
	handler_obj.wfile.write(f.read())
	return True

def post(path,args):
	#request for login
	if path=='/login':
		#send auth data using c sockets
		user = args['username'][0]
		passwd = args['password'][0]
		auth_str = pyjson.makeJSONStr('AUTH',user+'$'+passwd)	#need to use c functions
		len_str =  pyjson.makeLenStr(auth_str)
		pyio.write(len_str)
		pyio.write(auth_str)
		while not commons.got_auth_message:		#need to make this time dependent
			pass
		commons.got_auth_message = False
		#need to get the message
		auth_dict = commons.mq.get()
		#check status , then either redirect or deny
		if auth_dict['type']=='auth' and auth_dict['status']=='access':
			handler_obj.send_response(301)
			host = handler_obj.headers.get('Host')
       			new_path = '%s%s'%(host, 'gather')
      			handler_obj.send_header('Location', new_path)
       			handler_obj.end_headers()
		elif auth_dict['type']=='auth' and auth_dict['status']=='deny':
			pass #will handle soon
	return

def init(s):	
	global handler_obj
	handler_obj = s
