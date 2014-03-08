#!/usr/bin/python
from pylms import server
import sched, time
import urllib2
import urllib

#Show The Title of the Track played in the P-Space Logitech Server

#how frequently should it update (in seconds)
interval = 120

plmsserver = server.Server(hostname='IPADDRESS')
plmsserver.connect()
print plmsserver.get_players()
player=plmsserver.get_players()[0]

s = sched.scheduler(time.time, time.sleep)

def trackMonitorThread(sc):
  albumTitle = player.get_track_album()
  trackTitle = player.get_track_current_title()
  print "Title %s, Album %s" %(trackTitle,albumTitle)
  info = {"text":trackTitle}
  try:
    urllib2.urlopen("http://IPADDRESS/print/random/?"+urllib.urlencode(info).replace("+", "%20"))
  except:
    print "error"
  # do your stuff
  sc.enter(interval , 1, trackMonitorThread, (sc,))

s.enter(10, 1, trackMonitorThread, (s,))
s.run()
