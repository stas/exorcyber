"""
ExorCyber server
"""
import tornado.web
import tornadio
import tornadio.router
import tornadio.server

from threading import Thread

from os import path
from time import sleep
from Xlib import display

class StaticHandler(tornado.web.RequestHandler):
    """Regular HTTP handler to serve the static content"""
    def get(self):
        pass
        self.render("static/index.html")

class ExorCyberMouseHandler(Thread):
    peer = False
    last_x = False
    last_y = False
    screen_root = False
    
    def __init__(self, peer):
        self.peer = peer
        self.screen_root = display.Display().screen().root
        Thread.__init__(self)
    
    def run(self):
        while True:
            data = self.screen_root.query_pointer()._data
            
            if self.last_x != data["root_x"] or self.last_y != data["root_y"] :
                self.last_x = data["root_x"]
                self.last_y = data["root_y"]
                print "X:%d,Y:%d" % ( self.last_x, self.last_y )
                if self.peer:
                    self.peer.send( "X:%d,Y:%d" % ( self.last_x, self.last_y ) )

class ExorCyberConnection(tornadio.SocketConnection):
    def on_open(self, *args, **kwargs):
        print "New connection.\n"
        
        if not hasattr(self, 'mouse_handler'):
            self.mouse_handler = True
            ExorCyberMouseHandler( self ).start()
    
    def on_message(self, message):
        print "Message received: %s\n" % message
    
class ExorCyberServer:
    def __init__(self):
        file_root = path.normpath(path.dirname(__file__))
        router = tornadio.get_router(ExorCyberConnection)
        application = tornado.web.Application(
            [(r"/", StaticHandler),router.route()],
            enabled_protocols = [
                'websocket',
                'flashsocket',
            #    'xhr-multipart',
                'xhr-polling'
            ],
            socket_io_port = 8081,
            flash_policy_file = path.join(file_root, 'static/flashpolicy.xml'),
        )
        tornadio.server.SocketServer(application)

if __name__ == "__main__":
    try:
        ExorCyberServer()
    except KeyboardInterrupt:
        pass