WPN114
{
	classvar server;

	*server  { if(server.isNil) { ^Server.default } { ^server }; }
	*server_ { |target| server = target; }

	*vst_hdl { |out, node, plugname, netname|
		^WPN114_VST_HDL(out, node, plugname, netname)
	}

	*fields { |out, node, netname, sfpath, xfadelength|
		^WPN114_FIELDS(out, node, netname, sfpath, xfadelength);
	}

	*oneshots { |out, node, netname, sfpath, mts = false|
		^WPN114_ONESHOTS(out, node, netname, sfpath, mts);
	}

	*playlist { |out, netnode, netname, sfpath_array|
		^WPN114_PLAYLIST(out, netnode, netname, sfpath_array);
	}

	*rooms { |out, node, netname, n_inputs, n_outputs, lsarr|
		^WPN114_ROOMS(out, node, netname, n_inputs, n_outputs, lsarr);
	}

	*granary { |out, netnode, netname, sfpath|
		^WPN114_GRANARY(out, netnode, netname, sfpath);
	}

	*grainsc { |out, netnode, netname, sfpath|
		^WPN114_GRAINSC(out, netnode, netname, sfpath);
	}

	*master { |out_offset, node, netname, n_outputs|
		^WPN114_MASTER(out_offset, node, netname, n_outputs)
	}
}

WPN114_MODULE_BASE
{
	var m_out, m_nin, m_nout, m_netnode, m_name;
	var m_server, m_group, m_synths, m_def;
	var m_level;

	*new
	{
		|out = 0, n_inputs, n_outputs, netnode, netname|
		^this.newCopyArgs(out, n_inputs, n_outputs, netnode, netname)
	}

	module_base_ctor
	{
		m_server    = WPN114.server;
		m_netnode   = OSSIA.node(m_netnode, m_name);
		m_group     = Group(1, 'addToTail');

		m_level     = OSSIA.parameter(m_netnode, '/master/level', Float, [-96, 12], 0);

		OSSIA.parameter(m_netnode, '/master/vtrig', Impulse, critical: true)
		.callback = { |v| this.trigger };

		OSSIA.parameter(m_netnode, '/master/vkill', Integer, [0, inf], 'clip', critical: true)
		.callback = { |v| this.kill(v) };

		OSSIA.parameter(m_netnode, '/master/stop', Impulse, critical: true)
		.callback = { |v| this.clear };
	}

	net { ^m_netnode }

	level  { ^m_level.v }
	level_ { |v| m_level.v = v }

	n_inputs   { ^m_nin }
	n_inputs_  { |v| m_nin = v }

	n_outputs  { ^m_nout }
	n_outputs_ { |v| m_nout = v }

	connect
	{ |module|
		m_out = module.in;
		m_def.add;
	}

	trigger
	{
		m_synths = m_synths.add(
			Synth.head(m_group, m_name, m_netnode.snapshot));
	}

	kill
	{ |index|
		m_synths[index].free;
		m_synths.removeAt(index);
	}

	clear
	{
		m_group.freeAll;
		m_synths.clear()
	}

	save
	{ |path|
		m_netnode.save_preset(path);
	}

	recall
	{ |path|
		m_netnode.load_preset(path);
	}
}

WPN114_EFFECT_MODULE_BASE : WPN114_MODULE_BASE
{
	var m_inbus, m_mix;

	effect_base_ctor
	{
		m_inbus = Bus.audio(WPN114.server, m_nin);
		m_mix = OSSIA.parameter(m_netnode, '/master/mix', Float, [0, 1], 0.5, 'clip');
	}

	in
	{
		^m_inbus;
	}
}

WPN114_ROOMS : WPN114_EFFECT_MODULE_BASE
{
	var m_srcx, m_srcy, m_lsx, m_lsy, m_lsr, m_lsl;
	var m_stp;

	*new { |out, netnode, netname, n_inputs, n_outputs, lsarr|
		^super.new(out, n_inputs, n_outputs, netnode, netname)
		.module_base_ctor
		.effect_base_ctor
		.rooms_ctor(lsarr)
	}

	rooms_ctor { |lsarr|

		lsarr ?? { lsarr = Array.fill { |i| RoomsLS(i) }};
		m_mix.v = 1.0;

		m_srcx = OSSIA.parameter_array(m_nin, m_netnode,
			"/sources/%/position/xpos", Float, [0, 1], 0, 'clip');

		m_srcy = OSSIA.parameter_array(m_nin, m_netnode,
			"/sources/%/position/ypos", Float, [0, 1], 0, 'clip');

		m_lsx  = OSSIA.parameter_array(m_nout, m_netnode,
			"/speakers/%/position/xpos", Float, [0, 1], 0, 'clip');

		m_lsy  = OSSIA.parameter_array(m_nout, m_netnode,
			"/speakers/%/position/ypos", Float, [0, 1], 0, 'clip');

		m_lsr  = OSSIA.parameter_array(m_nout, m_netnode, "/speakers/%/radius", Float, [0, 1], 0.5);
		m_lsl  = OSSIA.parameter_array(m_nout, m_netnode, "/speakers/%/level", Float, [-96, 12], 1.0);

		// setup --------------------------------------------------------------------------------
		m_stp = RoomsSetup(m_server, m_nout, lsarr, { |buf|

			m_nout.do({|i|
				// update buffer values when value changes
				m_lsx[i].callback = { |v| m_stp.buffer.set(i*4,v) };
				m_lsy[i].callback = { |v| m_stp.buffer.set(i*4+1,v) };
				m_lsr[i].callback = { |v| m_stp.buffer.set(i*4+2,v) };
				m_lsl[i].callback = { |v| m_stp.buffer.set(i*4+3,v) };

				// updating parameter values
				m_lsx[i].v = lsarr[i].x;
				m_lsy[i].v = lsarr[i].y;
				m_lsr[i].v = lsarr[i].r;
				m_lsl[i].v = lsarr[i].l;
			});

			if(m_nin == 1) {
				m_def = SynthDef(m_name, {
					var rms = Rooms.ar(buf.bufnum, m_nout, InBus.ar(m_inbus, 1),
						m_srcx[0].kr, m_srcy[0].kr);
					Out.ar(m_out, rms);
				}).add;
			};

			if(m_nin == 2) {
				m_def = SynthDef(m_name, {
					var rms = Rooms2.ar(buf.bufnum, m_nout,
						InBus.ar(m_inbus, 1), InBus.ar(m_inbus, 1, 1),
						m_srcx[0].kr, m_srcy[0].kr, m_srcx[1].kr, m_srcy[1].kr);
					Out.ar(m_out, rms);
				}).add;
			};
		});
	}
}

WPN114_GRANARY : WPN114_MODULE_BASE
{
	var p_rfsh, p_gsize, p_rate, p_pos, p_pan;
	var p_randposfreq, p_randposwidth;
	var m_buf;

	*new { |out, netnode, netname, sfpath|
		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.granary_ctor(sfpath)
	}

	granary { |sfpath|

		m_buf = Buffer.read(WPN114.server, sfpath, action: { |buf|
			m_def = SynthDef(m_name, {
			}).add;
		});
	}
}



WPN114_ONESHOTS : WPN114_MODULE_BASE
{
	var m_buf, m_pan;
	*new { |out, netnode, netname, sfpath, mts = false|
		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.oneshots_ctor(sfpath, mts)
	}

	oneshots_ctor { |sfpath, mts|

		if(mts) {

			// if mono to stereo sample: add pan
			m_pan = OSSIA.parameter(m_netnode, 'pan', Float, [-1, 1], 0, 'clip');

			m_buf = Buffer.read(WPN114.server, sfpath, action: {
				m_def = SynthDef(m_name, {
					var f = PlayBuf.ar(m_buf.numChannels, m_buf, doneAction: 2);
					Out.ar(m_out, Pan2.ar(f, m_pan.kr, m_level.kr.dbamp));
				}).add;
			});

		} {
			// else normal
			m_buf = Buffer.read(WPN114.server, sfpath, action: {
				m_def = SynthDef(m_name, {
					var f = PlayBuf.ar(m_buf.numChannels, m_buf, doneAction: 2);
					Out.ar(m_out, f * m_level.kr.dbamp);
				}).add;
			});
		};
	}

	soundfile_ { |path|
		m_buf.free;
		m_buf = Buffer.read(WPN114.server, path, action: {
			m_def.add();
		});
	}
}

WPN114_PLAYLIST : WPN114_MODULE_BASE
{
	var m_buf_array;
	var <index, <loop, <gate;

	*new { |out, netnode, netname, sfpath_array|

		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.playlist_ctor(sfpath_array)
	}

	playlist_ctor { |sfpath_array|

		m_buf_array = [];
		m_def = [];

		index  = OSSIA.parameter(m_netnode, 'index', Integer, [0, sfpath_array.size - 1], 0, 'clip');
		loop   = OSSIA.parameter(m_netnode, 'loop', Boolean);
		gate   = OSSIA.parameter(m_netnode, 'gate', Integer, [0, 1], 0, 'clip');

		gate.callback = { |v|
			if(v == 1) { this.trigger(index.v) };
			m_group.set(\gate, v,);
		};

		sfpath_array.do({|sfpath,i|
			m_buf_array = m_buf_array.add(
				Buffer.read(WPN114.server, sfpath, action: { |buf|
					m_def = m_def.add(
						SynthDef(m_name ++ "_" ++ i, {
							var env = Env.adsr(sustainLevel: 1.0, releaseTime: 1);
							var envgen = EnvGen.kr(env, \gate.kr, doneAction: 2);
							var f = PlayBuf.ar(buf.numChannels, buf, loop: loop.kr, doneAction: 2);
							Out.ar(m_out, f * envgen * m_level.kr.dbamp);
						}).add;
					);
				});
			);
		});
	}

	trigger { |index|
		m_synths = m_synths.add(
			Synth.head(m_group, m_name ++ "_" ++ index, m_netnode.snapshot));
	}
}

WPN114_GRAINSC : WPN114_MODULE_BASE
{
	var <m_buf;
	var <freq, <grainsize, <rate, <pos, <pan;

	*new { |out, netnode, netname, sfpath|
		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.grainsc_ctor(sfpath);
	}

	grainsc_ctor { |sfpath|

		freq = OSSIA.parameter(m_netnode, 'freq', Float, [0.001, 100], 10, 'clip');
		grainsize = OSSIA.parameter(m_netnode, 'grainsize', Float, [0.001, 5], 1, 'clip');
		rate = OSSIA.parameter(m_netnode, 'rate', Float, [0.001, 10], 1, 'clip');
		pos = OSSIA.parameter(m_netnode, 'pos', Float, [0, 1], 0, 'clip');
		pan = OSSIA.parameter(m_netnode, 'pan', Float, [-1, 1], 0, 'clip');

		m_buf = Buffer.readChannel(WPN114.server, sfpath, channels: [0], action: { |buf|
			m_def = SynthDef(m_name, {
				var imp = Impulse.ar(freq.kr);
				var g = GrainBuf.ar(2, imp, grainsize.kr, buf.bufnum, rate.kr, pos.kr, 2, pan.kr,
					mul: m_level.kr.dbamp);
				Out.ar(m_out, g);
			}).add;
		});
	}

	soundfile_ { |sfpath|
		m_buf.free;
		m_buf = Buffer.readChannel(WPN114.server, sfpath, channels: [0], action: { |buf|
			m_def.add();
		});
	}
}


WPN114_FIELDS : WPN114_MODULE_BASE
{
	var m_xflen;
	var m_buf;

	*new { |out, netnode, netname, sfpath, xfade_length|
		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.fields_ctor(sfpath, xfade_length);
	}

	fields_ctor { |sfpath, xfl|

		m_xflen   = OSSIA.parameter(m_netnode, 'xfade_length', Integer, [0, inf], xfl, 'clip');
		m_buf     = Buffer.read(WPN114.server, sfpath, action: { |buf|
			buf.numFrames.postln;
			m_def = SynthDef(m_name, {
				var f = Fields.ar(buf.numChannels, buf.bufnum, m_xflen.kr);
				Out.ar(m_out, f * m_level.kr.dbamp)
			}).add
		});

		m_nout = m_buf.numChannels;
	}

	soundfile_ { |path|
		m_buf.free;
		m_buf = Buffer.read(WPN114.server, path, action: {
			m_def.add();
		});
	}
}


WPN114_SOUNDTRACKS : WPN114_MODULE_BASE
{
	var m_xflen, m_spos, m_epos;
	var m_buf;

	*new { |out, netnode, netname, sfpath, xfade_length|
		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.soundtracks_ctor(sfpath, xfade_length);
	}

	soundtracks_ctor { |sfpath, xfl| var buflen_ms;

		m_buf = Buffer.read(WPN114.server, sfpath, action: {
			m_def = SynthDef(m_name {
				var f = Soundtracks.ar(m_buf, m_xflen.kr, m_spos.kr, m_epos.kr, m_level.kr);
				Out.ar(m_out, f)}).add;
		});

		buflen_ms  = m_buf.numFrames/m_buf.sampleRate * 1000;

		m_xflen    = OSSIA.parameter(m_netnode, 'xfade_length', Integer, [0, inf], xfl, 'clip');
		m_spos     = OSSIA.parameter(m_netnode, 'startpos', Float, [0, buflen_ms], 0, 'clip');
		m_epos     = OSSIA.parameter(m_netnode, 'endpos', Float, [0, buflen_ms], buflen_ms, 'clip');
	}
}

WPN114_BURSTS : WPN114_MODULE_BASE
{
	var m_burstlen;

	*new { |out, netnode, netname, burstlen = 0.5|
		^super.new(out, 0, out.size, netnode, netname)
		.module_base_ctor
		.bursts_ctor(burstlen)
	}

	bursts_ctor { |burstlen|

		m_burstlen = OSSIA.parameter(m_netnode, 'length', Float, [0, 5], burstlen, 'low');

		m_def = SynthDef(m_name, {
			var b = PinkNoise.ar(m_level.kr.dbamp);
			Out.ar(m_out, b);
		}).add;
	}

	trigger
	{
		var out_array = m_out, target;
		fork {
			out_array.do({|i|
				m_out = i;
				m_def.add;
				target = Synth.head(m_group, m_name, m_netnode.snapshot);
				m_burstlen.v.wait;
				target.free;
			});
		};

		m_out = out_array;
	}
}

WPN114_VST_HDL : WPN114_EFFECT_MODULE_BASE
{
	var m_hdl, m_plugname;
	var m_mdnote_on, m_mdnote_off, m_mdcc, m_mdpc;
	var m_aftertouch, m_cpressure, m_pbend;

	*new { |out, netnode, plugname, netname|
		^super.new(out, 0, 0, netnode, netname)
		.module_base_ctor
		.effect_base_ctor
		.vst_hdl_ctor;
	}

	vst_hdl_ctor
	{
		// midi ------------------------------------------------------------------------------
		var midimap = [
			['note-off', 0x80, OSSIA.vec2f],
			['note-on', 0x90, OSSIA.vec2f],
			['afteroutch', 0xa0, OSSIA.vec2f],
			['continuous-control', 0xb0, OSSIA.vec2f],
			['patch-change', 0xc0, OSSIA.vec2f],
			['channel-pressure', 0xd0, Integer],
			['pitchbend', 0xe0, Integer]
		];

		midimap.do({|arr|
			OSSIA.parameter(m_netnode, '/midi/'++ arr[0], arr[2], [0, 127], 0, 'clip')
			.callback = { |v| m_hdl.midi([arr[1]] ++ v) };
		});

		// load plugin -----------------------------------------------------------------------
		m_hdl  = VSTPlug(m_plugname);
		m_nin  = m_hdl.num_inputs;
		m_nout = m_hdl.num_outputs;

		// parameters ------------------------------------------------------------------------
		m_hdl.parameters.do({|param|
			var pvalue = m_hdl.get(param);
			OSSIA.parameter(m_netnode, '/parameters/' ++ param, Float, [0, 1], pvalue, 'clip')
			.callback = { |v| m_hdl.set(param, v) };
		});

		// programs --------------------------------------------------------------------------
		m_hdl.programs.do({|program|
			OSSIA.parameter(m_netnode, '/programs/' ++ program, Impulse)
			.callback = { |v| m_hdl.load(program) };
		});

		// synthdef ---------------------------------------------------------------------------
		m_def = SynthDef(m_name, {
			var vst = VSTGen.ar(m_inbus[0], m_inbus[1], m_hdl.index, SampleRate.ir);
			Out.ar(m_out, vst);
		}).add;
	}

	show { AppClock.sched(0, { m_hdl.show() }) }

}