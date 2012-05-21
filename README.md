Slock with DPMS sucks less.
===========================

A bunch of commits ago slock (http://tools.suckless.org/slock) authors decided to remove DPMS 
screen blanking and I've started to put newer versions of slock into IgnorePkgs in pacman.conf.
I really like the "no more bloatware" philosophy, but having yet another shell script just to lock the screen 
and shut it down with DPMS looks like stone age not like elegant simplicity (especially since 
this bloatfull DPMS feature takes abominal amount of code: 3 lines). I'm having a bad day, ok? So I
moved slock into a real VCS (sorry mercurial, you suck more) and brought back the DPMS feature.
Also I'm a intolerant, xenophobic d!ck and I hate ponies.
