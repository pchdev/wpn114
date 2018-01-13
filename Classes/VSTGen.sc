VSTGen : MultiOutUGen {

	*ar { |vstplug, mul = 1.0, add = 0.0|
		^this.multiNew('audio', vstplug).madd(mul, add)
	}
}

VSTEffect : Filter {
	*ar { |vstplug, in, mul = 1.0, add = 0.0|
		^this.multiNew('audio', vstplug, in).madd(mul, add)
	}
}

VSTEffect2 : MultiOutUGen {
	*ar { |vstplug, in, in2, mul = 1.0, add = 0.0|
		^this.multiNew('audio', vstplug, in, in2).madd(mul, add)
	}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}

	checkInputs { ^this.checkNInputs(2); }
}