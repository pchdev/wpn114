
MIDILink {

	classvar <midiout;

	*init {
		MIDIClient.init;
		midiout = MIDIOut(0);
		midiout.latency = 0;
	}
}

QMIDI {
	*noteOn { |chn, index, val| MIDILink.midiout.noteOn(chn, index, val) }
	*noteOff{ |chn, index, val| MIDILink.midiout.noteOff(chn, index, val) }
	*control{ |chn, index, val| MIDILink.midiout.control(chn, index, val) }
	*program{ |chn, val| MIDILink.midiout.program(chn, val) }
}

Kaivo {

	var channel;

	*new { |channel|
		^this.newCopyArgs(channel);
	}

	*programs {
		var res  = (
			'ai_decay': 0,
			'ai_decay1': 1,
			'ai_decay2': 2,
			'brokenradio': 3,
			'dragontail': 4,
			'insects': 5,
			'fx_quarre': 6,
			'woodringer1': 7,
			'woodringer2': 8,
			'guitar_forest': 9,
			'guitar_quarre': 11,
			'guitar_temples': 12,
			'markhor': 12,
			'rainbells': 14,
			'rainbells2': 15,
			'rainpercs': 16,
			'woodclock': 17,
			'woodpercs': 18,
			'spring': 19,
		);

		^res;
	}

	on { |index, velocity|
		QMIDI.noteOn(channel, index, velocity);
	}

	off { |index, velocity|
		QMIDI.noteOff(channel, index, velocity);
	}

	program { |name|
		QMIDI.program(channel, Kaivo.programs[name]);
	}

	kill { 128.do({|i| QMIDI.noteOff(channel, i, 0)})}

	randchord { |times, offset, basenote, randscale, velocity|

		fork {
			times.do({
				QMIDI.noteOn(channel, basenote + randscale.rand, velocity);
				offset.wait;
			});
		}
	}

	scaledchord { |program, scale, offset, velocity|
		QMIDI.program(channel, Kaivo.programs[program]);
		scale.do({|note|
			QMIDI.noteOn(channel, note, velocity);
			offset.wait;
		});
	}


	quarre_whip { |user|

		QMIDI.program(channel, Kaivo.programs['guitar_quarre']);

		user.gsolo('whip', 'swipe');
		user.snsolo('accelerometer');

		user["whip"].assign({ this.randchord(2, 0.01, 52, 5, 50) });
		user["swipe"].assign({ this.kill });

		user["accelerometer"].assign({ |v|
			QMIDI.control(channel, 1, v[1].abs.linlin(0, 10, 0, 127));
			QMIDI.control(channel, 2, v[0].abs.linlin(0, 10, 0, 127));
		});

	}

	spring_whip { |user|

		QMIDI.program(channel, Kaivo.programs['spring']);
		QMIDI.control(channel, 3, 30);

		user.gsolo('whip', 'swipe');
		user.snsolo('accelerometer');

		user["whip"].assign({ this.randchord(2, 0.01, 60, 10, 50) });
		user["swipe"].assign({ this.kill });

		user["accelerometer"].assign({ |v|
			QMIDI.control(channel, 1, v[0].abs.linlin(0, 10, 0, 127));
			QMIDI.control(channel, 2, v[1].abs.linlin(0, 10, 0, 127));
		});

	}

	spring_percs { |user|

		QMIDI.program(channel, Kaivo.programs['spring']);
		QMIDI.control(channel, 3, 0);

		user.gsolo('whip', 'swipe');
		user.snsolo('accelerometer');

		user["whip"].assign({ this.randchord(2, 0.01, 50, 10, 50) });
		user["swipe"].assign({ this.kill });

		user["accelerometer"].assign({ |v|
			QMIDI.control(channel, 1, v[0].abs.linlin(0, 10, 0, 127));
			QMIDI.control(channel, 2, v[1].abs.linlin(0, 10, 0, 127));
		});
	}

	temples { |user|
		user.reset();
		user.gsolo('turnover', 'swipe');
		user.snsolo('rotation');

		QMIDI.program(channel, Kaivo.programs['guitar_temples']);
		user["rotation"].assign({|v|
			QMIDI.control(channel, 1, v[0].abs.linlin(0, 90, 0, 127));
			QMIDI.control(channel, 2, v[1].abs.linlin(0, 90, 0, 127));
			QMIDI.control(channel, 3, v[2].linlin(-180, 180, 0, 127));
		});

		user["swipe"].assign({
			this.randchord(2+3.rand, 0.1.linrand, 60, 10, 20)
		});

		user["turnover"].assign({this.kill});
	}

	rainbells { |user|
		user.reset();
		user.gsolo('shake', 'swipe');
		user.snsolo('proximity', 'rotation');
		QMIDI.program(channel, Kaivo.programs['rainbells']);
		user["proximity"].assign({ |v|
			if(v) { QMIDI.noteOn(channel, 60 + 20.rand, 20) }
		});

		user["rotation"].assign({|v|
			QMIDI.control(channel, 1, v[0].abs.linlin(0, 85, 0, 127));
			QMIDI.control(channel, 2, v[1].abs.linlin(0, 90, 0, 127));
			QMIDI.control(channel, 3, v[2].linlin(-180, 180, 0, 127));

		});

		user["swipe"].assign({this.kill});

		user["shake"].assign({this.kill});
		user["shake/left"].assign({this.kill});
		user["shake/right"].assign({this.kill});
		user["shake/up"].assign({this.kill});
		user["shake/down"].assign({this.kill});
	}

	insects { |user|

		QMIDI.program(channel, Kaivo.programs['insects']);

		user.gsolo('cover', 'swipe');
		user.snsolo('rotation');

		user["cover"].assign({ this.randchord(2, 0.01, 70, 20, 127) });
		user["swipe"].assign({ this.kill });

		user["rotation"].assign({ |v|
			QMIDI.control(channel, 1, v[0].abs.linlin(0, 90, 0, 127));
			QMIDI.control(channel, 2, v[1].abs.linlin(0, 90, 0, 127));
			QMIDI.control(channel, 3, v[2].linlin(-180, 180, 0, 127));
		});

	}

	dragontail { |user|
		user.reset();
		user.gsolo('cover');
		user.snsolo('rotation');
		user["cover"].assign( { |v|
			QMIDI.program(channel, Kaivo.programs['dragontail']);
			QMIDI.noteOn(channel, 60+ 10.rand, 40) });
		user["rotation"].assign( { |v|
			QMIDI.control(channel, 38, (v[2].abs/180) * 127);
		});
	}
}

Falcon {

	var channel;

	*new { |channel|
		^this.newCopyArgs(channel);
	}

	softsynth { |user|
		user.reset();
		user.gsolo('cover', 'swipe');
		user["cover"].assign({
			QMIDI.noteOn(channel, 50, 100)
		});

		user["swipe"].assign({
			this.kill();
		});

		user.snsolo('rotation');
		user["rotation"].assign({|v|
			QMIDI.control(channel,1, v[0].abs.linlin(0, 90, 0, 127));
			QMIDI.control(channel, 7, v[1].abs.linlin(0, 90, 0, 127));
		});
	}

	kill { 128.do({|i| QMIDI.noteOff(channel, i, 0)})}
}

Absynth {

	var channel;

	*new { |channel|
		^this.newCopyArgs(channel);
	}

	kill { 128.do({|i| QMIDI.noteOff(channel, i, 0)})}

}
		