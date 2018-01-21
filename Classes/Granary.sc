Fields : MultiOutUGen {

	*ar { |num_channels, bufnum, xfade_length=65536|
		^this.multiNew('audio', num_channels, bufnum, xfade_length)
	}

	init { |num_channels, bufnum, xfade_length=65536|
		inputs = [bufnum, xfade_length];
		^this.initOutputs(num_channels, rate)
	}
}
