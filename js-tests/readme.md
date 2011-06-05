ExorCyber SocketIO Test App
===========================

### Requirements
* Python >=2.7
* Virtualenv
* Tornado 1.2.1
* Tornadio 0.0.4
* Python Xlib

### Setup
```
cd /tmp
virtualenv socketio --no-site-packages
pip install tornado
pip install tornadio
```

Xlib comes as a shared precompiled module,
on Ubuntu I found it in `/usr/share/pyshared/Xlib`.

```
ln -s /usr/share/pyshared/Xlib socketio/lib/python2.7/Xlib
cd socketio
```

Now run

```
git clone ...
cd whereyoudownloadedthefiles
python server.py
```

Now go to `localhost:8081`

I tested this in Chromium/Chrome, but should also work in Firefox4.