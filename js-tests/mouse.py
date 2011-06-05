"""
Gets the mouse X/Y position
"""

from Xlib import display

def mousepos():
    last_x = False
    last_y = False
    try:
        screen_root = display.Display().screen().root
        while True:
            data = screen_root.query_pointer()._data
            if last_x != data["root_x"] or last_y != data["root_y"] :
                last_x = data["root_x"]
                last_y = data["root_y"]
                print "X:%d,Y:%d" % ( last_x, last_y )
        
    except KeyboardInterrupt:
        return

if __name__ == "__main__":
    format( mousepos() )