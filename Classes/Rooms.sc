RoomsSetup {

	var <s, <nchannels, <lsarr;
	var <buffer;

	*new {|s, nchannels, lsarr|
		^super.newCopyArgs(s,nchannels,lsarr).init
	}

	init {
		var arr = [];

		nchannels.do({|i|
			var ls = lsarr[i];
			4.do({|i| arr = arr.add(ls.instVarAt(i+1)) });
		});

		buffer = Buffer.loadCollection(s, arr, 1);
	}

	bufnum { ^buffer.bufnum; }

}

RoomsLS {
		var <>i, <>x, <>y, <>r, <>l;
		*new { |index = 0, x = 0.0, y = 0.0, r = 0.5, l = 1.0|
			^this.newCopyArgs(index,x,y,r,l)
		}
	}

RoomsHMI {
	var w, ls_arr;

	*new {
		^super.new.init
	}

	init {

		w = Window("RoomsHMI", Rect(200, 200, 1024, 768), false);


	}

}

Rooms : MultiOutUGen {

	*ar { |bufnum, nchannels = 2, in = 0.0, x = 0.5, y = 0.5|
		^this.multiNew('audio', bufnum, nchannels, in, x, y)
	}

	init { |bufnum, nchannels, in, x, y|
		inputs = [bufnum, nchannels, in, x, y];
		^this.initOutputs(nchannels, rate)
	}
}

Rooms2 : MultiOutUGen {
	*ar { |bufnum, nchannels = 2, in_1 = 0.0, in_2 = 0.0, x1 = 0.25, y1 = 0.5, x2 = 0.75, y2 = 0.5|
		^this.multiNew('audio', bufnum, nchannels, in_1, in_2, x1, y1, x2, y2)
	}

	init { |bufnum, nchannels, in_1, in_2, x1, y1, x2, y2|
		inputs = [bufnum, nchannels, in_1, in_2, x1, y1, x2, y2];
		^this.initOutputs(nchannels, rate)
	}
}
