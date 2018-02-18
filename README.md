# Historical notice

This project is mostly defunct, but I would like to keep a record of it since it was one of my very first open source projects back in 2004!
It was very relevant when I was eager to broadcast the various types of music I liked to whoever would listen.

# What is TagReport?

Winamp for Windows had a neat feature where it could output its playlist to
a HTML file. TagReport came to be simply because [xmms](http://www.xmms.org/),
the UNIX world's Winamp, does not have this kind of capability.

TagReport handles scanning multiple entire directory hierarchies, but
currently can only handle reading MP3s and Ogg Vorbis audio files, unlike
xmms which can be made to read everything under the sun with the right plugins.

Also, the FLAC (Free Lossless Audio Codec) is supported if the libFLAC library
is available on your system at build time.

It also has support for customizable HTML templates. For more information
on this simple file format look at [def/sample.def](def/sample.def).

It uses the next-generation TagLib by Scott Wheeler to accomplish this, which
is still in its nascent stages, so cross-platform compatibility is still
fairly weak. If you need to install this (which you probably do), read the
INSTALL file for more information.

(And failing this, it may use libvorbisfile and libid3tag if they are
available on your system. See [INSTALL](INSTALL) for more details.

It was developed on Linux, the initial target platform for TagLib, but appears
to work perfectly on FreeBSD as well thus far.

# Feedback

I welcome feedback on TagReport. Please send me email at joshk@triplehelix.org,
or if you want to reach Scott Wheeler about TagLib, wheeler@kde.org.
