QuarreUser {

	var <index, parent, <name, connected;
	var m_sensors, m_gestures;

	*new { |index, parent|
		^this.newCopyArgs(index, parent).init;
	}

	device { ^parent.device }

	gesture_list {
		^[
			"whip",
			"cover",
			"turnover",
			"freefall",
			"twist",
			"shake",
			"pickup"
		]
	}

	at { |element|
		var res;
		m_gestures.do({|gesture|
			if(element == gesture.id) { res = gesture };
		});

		res ?? { m_sensors.do({|sensor|
			if(element == sensor.id) { res = sensor };
		});
		}

		^res
	}

	init {

		m_gestures   = [ ];

		connected = OSSIA.parameter(parent.device, format("/user/%/connected", index),
			Boolean, critical: true).callback = { |v|
			parent.update_user_status(index, v);
		};

		name = OSSIA.parameter(parent.device, format("/user/%/name", index),
			String, critical: true).callback = { |v|
			parent.update_user_names(index, v);
		};

		m_sensors = [
			QuarreSensor(this, "accelerometer", OSSIA.vec3f),
			QuarreSensor(this, "rotation", OSSIA.vec3f),
			QuarreSensor(this, "proximity", Boolean)
		];


		this.gesture_list.do({|gesture, i|
			m_gestures = m_gestures.add(
				QuarreGesture(this, gesture)
			);
		});

	}

	reset {

		m_sensors.do({|sensor|
			sensor.deactivate();
		});

		m_gestures.do({|gesture|
			gesture.deactivate();
		});

	}

	sensors { |... ids|
		var arr = [];
		ids.do({|id|
			m_sensors.do({|sensor|
				if(id == sensor.id) { arr = arr.add(sensor) }
			});
		});

		^QuarrePhoneElementArray(arr);
	}

	gestures { |... ids|
		var arr = [];
		ids.do({|id|
			m_gestures.do({|gesture|
				if(id == gesture.id) { arr = arr.add(gesture) }
			});
		});

		^QuarrePhoneElementArray(arr);
	}

	gsolo { |... ids|
		m_gestures.do({|gesture|
			if(ids.includes(gesture.id.asSymbol))
			{ gesture.activate() } { gesture.deactivate() }
		});
	}

	snsolo { |... ids|

		m_sensors.do({|sensor|
			if(ids.includes(sensor.id.asSymbol)) { sensor.activate() } { sensor.deactivate() }
		});

	}

	sensor { |sensor_id|
		^m_sensors[m_sensors.indexOf(sensor_id)];
	}

	gesture { |gesture_id|
		^m_gestures[m_gestures.indexOf(gesture_id)];
	}

}

QuarrePhoneElementArray {
	var array;

	*new { |elements|
		^this.newCopyArgs(elements)
	}

	at { |i| ^array[i] }

	activate { |solo = true|
		array.do({|element|
			element.activate()
		});
	}

	deactivate { |solo = true|
		array.do({|element|
			element.deactivate()
		});
	}

	assign { |func|
		array.do({|element|
			element.assign(func)
		});
	}
}

QuarrePhoneElement {

	var <parent, <type, <id, <datatype;
	var active, available, data;

	assign { |function|
		data.callback = function;
	}

	activate { |solo = true|
		active.v = true;
	}

	deactivate {
		active.v = false;
	}

	available { ^available.v }
	data { ^data.v }

	format_addr { |parameter_name|
		var res = format("/user/%/%/%/%", parent.index, type, id, parameter_name);
		^res
	}

}

QuarreSensor : QuarrePhoneElement {

	*new { |parent, id, datatype|
		^super.newCopyArgs(parent, "sensors", id, datatype).init;
	}

	init {

		active      = OSSIA.parameter(parent.device, this.format_addr("active"), Boolean, critical: true);
		available   = OSSIA.parameter(parent.device, this.format_addr("available"), Boolean, critical: true);
		data        = OSSIA.parameter(parent.device, this.format_addr("data"), datatype, critical: true);

	}

}

QuarreGesture : QuarrePhoneElement {

	*new { |parent, id|
		^super.newCopyArgs(parent, "gestures", id).init;
	}

	init {

		active      = OSSIA.parameter(parent.device, this.format_addr("active"), Boolean, critical: true);
		available   = OSSIA.parameter(parent.device, this.format_addr("available"), Boolean, critical: true);
		data        = OSSIA.parameter(parent.device, this.format_addr("trigger"), Impulse, critical: true);

	}

}

QuarreServer {

	var users_max, <users, <user_names, <user_status;
	var <device;
	var available_slot;
	var scenario_start, scenario_end, scene_name;
	var <server_quit;

	*new { |users_max|
		^this.newCopyArgs(users_max).init
	}

	at { |i|
		var res;
		users.do({|user|
			if(user.name.v == i) { res = user };
		});

		^res;
	}

	put { |i, v|

	}

	nextAvailableSlot {
		var slot = user_status.indexOf(false);
		slot !? { available_slot.v = slot };
		slot ?? { available_slot.v = -1 };
	}

	update_user_status { |index, value|
		user_status[index] = value;
		this.nextAvailableSlot();
		user_status.postln;
	}

	update_user_names { |index, value|
		if(value == "") { value = nil };
		user_names[index] = value;
		user_names.postln;
	}

	init {

		// put the quit signal on top, to avoid ossia cleanup coming before it...
		ShutDown.objects.insert(0, { this.server_quit.v = 1 });

		user_status = Array.fill(users_max, { false });
		user_names  = Array.fill(users_max, { nil });

		device = OSSIA.device("quarre-server").exposeOSCQueryServer(1234, 5678, {

			// global
			available_slot    = OSSIA.parameter(device, '/slots/available',
				                               Integer, [-1, users_max], 0, 'clip', true);

			scenario_start    = OSSIA.parameter(device, '/scenario/start', Integer, critical: true);
			scenario_end      = OSSIA.parameter(device, '/scenario/end', Integer, critical: true);
			scene_name        = OSSIA.parameter(device, '/scenario/scene/name', String, critical: true);
			server_quit       = OSSIA.parameter(device, '/server/quit', Integer, nil, 0, critical: true);

			users             = Array.fill(users_max, { |i| QuarreUser(i, this) });

			/*// interactions
			interaction_incoming = OSSIA.parameter_array(users_max, device,
				"/user/%/interactions/next/incoming", List, critical: true);

			interaction_begin = OSSIA.parameter_array(users_max, device,
				"/user/%/interactions/next/begin", List, critical: true);

			interaction_end = OSSIA.parameter_array(users_max, device,
				"/user/%/interactions/current/end", Integer, critical: true);

			interaction_force = OSSIA.parameter_array(users_max, device,
				"/user/%/interactions/current/force", Integer, critical: true);*/

		});
	}
}

