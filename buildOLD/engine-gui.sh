#!/bin/bash

[[ ! -d "../assets/engines/custom" ]] && mkdir ../assets/engines/custom

yad     --info \
        --text="This script wil automatically generate an inline engine from your dimensions and \"install\" it on main.mr" \
        --title="Engine Builder"

accepted=$?
if ((accepted != 0)); then
    exit 1
fi

OUTPUT=$(yad --form \
    --field="Engine Name" \
    --field="Node Name (no spaces and special chars)" \
    --field="Piston Count" \
    --field="Bore (mm)" \
    --field="Stroke (mm)" \
    --field="Rod Ratio" \
    --field="Compression Height (mm)" \
    --field="Deck Clearance (mm)" \
    --field="Piston Head Volume (cc)" \
    --field="Redline (RPM)" \
    --field="Rev-Limit (RPM)" \
    --field="Cam Lift intake (mm)" \
    --field="Lobe Duration intake (deg)" \
    --field="Cam Lift exhaust (mm)" \
    --field="Lobe Duration exhaust (deg)" \
    --title="Engine Builder" \
    --text="Enter Engine Dimensions" \
    --separator="," \
    "" \
    "" \
    "1" \
    "86" \
    "71.6" \
    "1.4" \
    "30" \
    "10" \
    "30" \
    "6700" \
    "7200" \
    "17" \
    "195" \
    "17" \
    "195" \
)

accepted=$?
if ((accepted != 0)); then
    exit 1
fi

name=$(awk -F, '{print $1}' <<<$OUTPUT)
node=$(awk -F, '{print $2}' <<<$OUTPUT)
cyl=$(awk -F, '{print $3}' <<<$OUTPUT)
bore=$(awk -F, '{print $4}' <<<$OUTPUT)
stroke=$(awk -F, '{print $5}' <<<$OUTPUT)
rod_ratio=$(awk -F, '{print $6}' <<<$OUTPUT)
comp_height=$(awk -F, '{print $7}' <<<$OUTPUT)
deck_clearance=$(awk -F, '{print $8}' <<<$OUTPUT)
chamber_volume=$(awk -F, '{print $9}' <<<$OUTPUT)
redline=$(awk -F, '{print $10}' <<<$OUTPUT)
rev_limit=$(awk -F, '{print $11}' <<<$OUTPUT)
cam_lift1=$(awk -F, '{print $12}' <<<$OUTPUT)
lobe_dur1=$(awk -F, '{print $13}' <<<$OUTPUT)
cam_lift2=$(awk -F, '{print $14}' <<<$OUTPUT)
lobe_dur2=$(awk -F, '{print $15}' <<<$OUTPUT)

text=""

[[ ! "$name" =~ ^[a-zA-Z0-9[:space:]_]+?$ ]] && 
text="$text\nName must contain atleast 1 Letter." &&
error=1

[[ ! "$node" =~ ^[a-z]+([0-9a-z]+)?$ ]] &&
text="$text\nNode must only contain lowercase letters, numbers and must only start with a letter." &&
error=1

[[ ! "$cyl" =~ ^[0-9]+$ ]] &&
text="$text\nCylinder Amount must only contain an integer." &&
error=1

[[ ! "$bore" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nBore must only contain a number with optional decimal point." &&
error=1

[[ ! "$stroke" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nStroke must only contain a number with optional decimal point." &&
error=1

[[ ! "$rod_ratio" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nRod Ratio must only contain number with optional decimal point." &&
error=1

[[ ! "$comp_height" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nCompression Height must only contain number with optional decimal point." &&
error=1

[[ ! "$deck_clearance" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nDeck Clearance must only contain number with optional decimal point." &&
error=1 

[[ ! "$chamber_volume" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nChamber Volume must only contain number with optional decimal point." &&
error=1

[[ ! "$redline" =~ ^[0-9]+$ ]] &&
text"$text\nRedline must only contain an integer." &&
error=1

[[ ! "$rev_limit" =~ ^[0-9]+$ ]] &&
text"$text\nRev-Limit must only contain an integer." &&
error=1

[[ ! "$cam_lift1" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nCam Lift intake must only contain number with optional decimal point." &&
error=1

[[ ! "$lobe_dur1" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nLobe Duration intake must only contain number with optional decimal point." &&
error=1

[[ ! "$cam_lift2" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nCam Lift exhaust must only contain number with optional decimal point." &&
error=1

[[ ! "$lobe_dur2" =~ ^[0-9]+([.][0-9]+)?$ ]] && 
text="$text\nLobe Duration exhaust must only contain number with optional decimal point." &&
error=1

if [[ $error = 1 ]]; then
    yad --info --title="Error" --text="$text"
    exit 1
fi

pi=3.14159265358979323846264338327950288419716939937510

engine_volume=$(echo "($pi*(($bore/10)/2)^2*($stroke/10))*$cyl" | bc -l | awk '{print int($1+0.5)}')

max_intake_cfm=$(echo "$rev_limit*($engine_volume/16.387)/3456*1.05" | bc -l | awk '{print int($1+0.5)}')

firing_order=$(
    for i in $(seq 1 1 $cyl); do
        [[ $i -eq $cyl ]] &&
        echo -n "$i" ||
        echo -n "$i,"
    done
)

firing_order=$(yad --form \
    --field="Firing Order ($cyl Cylinders)" \
    --title="Engine Builder" \
    --text="Enter Firing order" \
    --separator="" \
    "$firing_order" \
)

accepted=$?
if ((accepted != 0)); then
    exit 1
fi

firing_order=$(echo $firing_order | tr "," "\n")
firing_orderarr=( $(echo $firing_order | tr "," "\n") )
firing_count=0

for i in "${firing_orderarr[@]}"; do
    firing_count=$(($firing_count+1))
done

[[ ! $firing_count -eq $cyl ]] &&
yad --info --title="Error" --text="Firing Order must contain $cyl numbers."

# start of file generation

y=0
x=0
z=0

rm ../assets/engines/custom/*

rot=$(echo "360/$cyl")

printf "
import \"engine_sim.mr\"

units units()
constants constants()
impulse_response_library ir_lib()

label cycle(720 * units.deg)
label piston_amt($cyl)
label rot($rot * units.deg)

label e_stroke($stroke) //mm 
label e_bore($bore) //mm
label rod_ratio($rod_ratio)
label comp_height($comp_height) //mm
label deck_clearance($deck_clearance) //mm
label e_chamber_volume($chamber_volume) //cc
label e_throw(e_stroke * units.m / 2)
label e_deck_height(deck_clearance+(e_throw+e_stroke*rod_ratio+comp_height))
label crank_x(0)
label crank_y(0)

public node bmw_distributor {
    input wires;
    input timing_curve;
    input rev_limit: 8500 * units.rpm;
    alias output __out:
        ignition_module(timing_curve: timing_curve, rev_limit: rev_limit, limiter_duration: 0.08 * units.sec)
\n" > "../assets/engines/custom/$(echo $node).mr"

# distributor angles

g=1

for i in $firing_order; do

    angle=$(echo "($i-1)/$cyl * 720" | bc -l | awk '{print int($1+0.5)}')

    [[ $g -eq $cyl ]] &&
    echo "            .connect_wire(wires.wire$g, $angle * units.deg);" >> "../assets/engines/custom/$(echo $node).mr"||
    echo "            .connect_wire(wires.wire$g, $angle * units.deg)" >> "../assets/engines/custom/$(echo $node).mr"

    g=$(($g+1))
done

printf "
}

private node wires {\n" >> "../assets/engines/custom/$(echo $node).mr"

# add wires

for i in $(seq 1 1 $cyl); do

    echo "output wire$i: ignition_wire();" >> "../assets/engines/custom/$(echo $node).mr"

done

printf "
}

private node add_sym_sample {
    input angle;
    input lift;
    input this;
    alias output __out: this;

    this.add_sample(angle * units.deg, lift * units.thou)
    this.add_sample(-angle * units.deg, lift * units.thou)
}

public node m52b28_lobe_profile_int {
    alias output __out:
        harmonic_cam_lobe(
            duration_at_50_thou: $lobe_dur1 * units.deg,
            gamma: 0.95,
            lift: $cam_lift1 * units.mm,
            steps: 100
        );
}

public node m52b28_lobe_profile_exh {
    alias output __out:
        harmonic_cam_lobe(
            duration_at_50_thou: $lobe_dur2 * units.deg,
            gamma: 0.9,
            lift: $cam_lift2 * units.mm,
            steps: 100
        );
}

public node bmw_camshaft_builder {
    input lobe_profile: m52b28_lobe_profile_int();
	input ex_lobe_profile: m52b28_lobe_profile_exh();
    input intake_lobe_profile: lobe_profile;
    input exhaust_lobe_profile: ex_lobe_profile;
    input lobe_separation: 115.0 * units.deg;
    input intake_lobe_center: lobe_separation;
    input exhaust_lobe_center: 105.0 * units.deg;
    input advance: 50.0 * units.deg;
    input base_radius: 19 * units.mm;

    output intake_cam_0: _intake_cam_0;
    output exhaust_cam_0: _exhaust_cam_0;

    camshaft_parameters params(
        advance: advance,
        base_radius: base_radius
    )

    camshaft _intake_cam_0(params, lobe_profile: intake_lobe_profile)
    camshaft _exhaust_cam_0(params, lobe_profile: exhaust_lobe_profile)


    label rot60(60 * units.deg)
    label rot90(90 * units.deg)
    label rot120(120 * units.deg)
    label rot180(180 * units.deg)
    label rot360(360 * units.deg)
\n" >> "../assets/engines/custom/$(echo $node).mr"


y=0
x=0
z=0

# intake cams

echo "    _intake_cam_0" >> "../assets/engines/custom/$(echo $node).mr"

for i in $firing_order; do

    angle=$(echo "($i-1)*$rot*2" | bc -l | awk '{print int($1+0.5)}')
    echo "        .add_lobe(rot360 + intake_lobe_center + $angle * units.deg)" >> "../assets/engines/custom/$(echo $node).mr"

done

y=0
x=0
z=0

# exhaust cams

echo "    _exhaust_cam_0" >> "../assets/engines/custom/$(echo $node).mr"

for i in $firing_order; do

    angle=$(echo "($i-1)*$rot*2" | bc -l | awk '{print int($1+0.5)}')
    echo "        .add_lobe(rot360 - exhaust_lobe_center + $angle * units.deg)" >> "../assets/engines/custom/$(echo $node).mr"

done

y=0
x=0
z=0

printf "
}

private node add_flow_sample {
    input lift;
    input flow;
    input this;
    alias output __out: this;

    this.add_sample(lift * units.mm, k_28inH2O(flow))
}

public node bmw_m52b28_head {
    input intake_camshaft;
    input exhaust_camshaft;
    input chamber_volume: e_chamber_volume * units.cc;
    input flip_display: false;
	
	input flow_attenuation: 1.0;
    input lift_scale: 1.0;
    alias output __out: head;

    function intake_flow(1 * units.mm)
    intake_flow
        .add_flow_sample(0 * lift_scale, 0 * flow_attenuation)
        .add_flow_sample(1 * lift_scale, 35 * flow_attenuation)
        .add_flow_sample(2 * lift_scale, 60 * flow_attenuation)
        .add_flow_sample(3 * lift_scale, 90 * flow_attenuation)
        .add_flow_sample(4 * lift_scale, 125 * flow_attenuation)
        .add_flow_sample(5 * lift_scale, 150 * flow_attenuation)
        .add_flow_sample(6 * lift_scale, 175 * flow_attenuation)
        .add_flow_sample(7 * lift_scale, 200 * flow_attenuation)
        .add_flow_sample(8 * lift_scale, 215 * flow_attenuation)
        .add_flow_sample(9 * lift_scale, 230 * flow_attenuation)
        .add_flow_sample(10 * lift_scale, 235 * flow_attenuation)
        .add_flow_sample(11 * lift_scale, 235 * flow_attenuation)
        .add_flow_sample(12 * lift_scale, 238 * flow_attenuation)

    function exhaust_flow(1 * units.mm)
    exhaust_flow
        .add_flow_sample(0 * lift_scale, 0 * flow_attenuation)
        .add_flow_sample(1 * lift_scale, 35 * flow_attenuation)
        .add_flow_sample(2 * lift_scale, 55 * flow_attenuation)
        .add_flow_sample(3 * lift_scale, 85 * flow_attenuation)
        .add_flow_sample(4 * lift_scale, 105 * flow_attenuation)
        .add_flow_sample(5 * lift_scale, 120 * flow_attenuation)
        .add_flow_sample(6 * lift_scale, 140 * flow_attenuation)
        .add_flow_sample(7 * lift_scale, 150 * flow_attenuation)
        .add_flow_sample(8 * lift_scale, 155 * flow_attenuation)
        .add_flow_sample(9 * lift_scale, 160 * flow_attenuation)
        .add_flow_sample(10 * lift_scale, 165 * flow_attenuation)
        .add_flow_sample(11 * lift_scale, 165 * flow_attenuation)
        .add_flow_sample(12 * lift_scale, 165 * flow_attenuation)

		
    cylinder_head head(
        chamber_volume: chamber_volume,
        intake_runner_volume: 100.0 * units.cc,
        intake_runner_cross_section_area: 2 * 12.4087 * units.cm2,

        intake_port_flow: intake_flow,
        exhaust_port_flow: exhaust_flow,
        intake_camshaft: intake_camshaft,
        exhaust_camshaft: exhaust_camshaft,
        flip_display: flip_display
    )
}

public node $node {
    alias output __out: engine;

    engine engine(
        name: \"$name\",
        starter_torque: 50000 * units.lb_ft,
        starter_speed: 800 * units.rpm,
        redline: $redline * units.rpm,
        fuel: fuel(
            max_turbulence_effect: 1.0,
            burning_efficiency_randomness: 0,
            max_burning_efficiency: 1.0
        ),
        throttle_gamma: 2.0
    )

    wires wires()

    crankshaft c0(
        throw: e_stroke * units.mm / 2,
        flywheel_mass: 5.9 * units.kg,
        mass: 5 * units.kg,
        friction_torque: 1 * units.lb_ft,
        moment_of_inertia: 0.22986844776863666 * 0.3,
        position_x: crank_x * units.mm,
        position_y: crank_y * units.mm,
        tdc: 120.0 * units.deg
    )\n" >> "../assets/engines/custom/$(echo $node).mr"

# set rod journals

g=1

for i in $firing_order; do

    [[ $i -gt $(echo $cyl/2 | bc -l | awk '{print int($1+0.5)}') ]] &&
    angle=$(echo "($i-1-$cyl/2)*$rot*2" | bc -l | awk '{print int($1+0.5)}') ||
    angle=$(echo "($i-1)*$rot*2" | bc -l | awk '{print int($1+0.5)}')
    echo "    rod_journal rj$(($g-1))(angle: $angle * units.deg)" >> "../assets/engines/custom/$(echo $node).mr" 

    g=$(($g+1))

done

echo "    c0" >> "../assets/engines/custom/$(echo $node).mr"

# add rod journals

for i in $(seq 1 1 $cyl); do 
    echo "        .add_rod_journal(rj$(($i-1)))" >> "../assets/engines/custom/$(echo $node).mr"
done

printf "
    piston_parameters piston_params(
        mass: 280 * units.g,
        //blowby: k_28inH2O(0.1),
        compression_height: comp_height * units.mm,
        wrist_pin_position: 0.0,
        displacement: 0.0
    )

    connecting_rod_parameters cr_params(
        mass: 300.0 * units.g,
        moment_of_inertia: 0.0015884918028487504,
        center_of_mass: 0.0,
        length: e_stroke * rod_ratio * units.mm
    )

    cylinder_bank_parameters bank_params(
        bore: e_bore * units.mm,
        deck_height: e_deck_height * units.mm
    )

    performer_rpm_intake intake(
        carburetor_cfm: $max_intake_cfm.0,
        idle_flow_rate_cfm: 0,
        idle_throttle_plate_position: 0.994
    )

    exhaust_system_parameters es_params(
        outlet_flow_rate: k_carb($(echo "$max_intake_cfm*1.02" | bc -l | awk '{print int($1+0.5)}').0),
        primary_tube_length: 500.0 * units.inch,
        primary_flow_rate: k_carb($(echo "$max_intake_cfm*1.05" | bc -l | awk '{print int($1+0.5)}').0),
        velocity_decay: 0.65, //0.5
        volume: 500.0 * units.L
    )

    exhaust_system_parameters es_params2(
        outlet_flow_rate: k_carb($(echo "$max_intake_cfm*1.02" | bc -l | awk '{print int($1+0.5)}').0),
        primary_tube_length: 400.0 * units.inch,
        primary_flow_rate: k_carb($(echo "$max_intake_cfm*1.05" | bc -l | awk '{print int($1+0.5)}').0),
        velocity_decay: 0.65, //0.5
        volume: 300.0 * units.L
    )

    impulse_response ir0(filename: \"../../sound-library/smooth/smooth_39.wav\", volume: 0.01)
	impulse_response ir1(filename: \"../../sound-library/smooth/smooth_32.wav\", volume: 0.0096)
    exhaust_system exhaust0(es_params, audio_volume: 1.0, impulse_response: ir0)
    exhaust_system exhaust1(es_params2, audio_volume: 0.96, impulse_response: ir1)
 
    cylinder_bank b0(bank_params, angle: 0 * units.deg)
    b0\n" >> "../assets/engines/custom/$(echo $node).mr"

for i in $(seq 1 1 $cyl); do

    [[ $(($i % 2)) -eq 0 ]] && exhaust="exhaust0"
    [[ $(($i % 2)) -eq 1 ]] && exhaust="exhaust1"

    printf "
        .add_cylinder(
            piston: piston(piston_params, blowby: k_28inH2O(0.1)),
            connecting_rod: connecting_rod(cr_params),
            rod_journal: rj$(($i-1)),
            intake: intake,
            exhaust_system: $exhaust,
            ignition_wire: wires.wire$i
        )\n" >> "../assets/engines/custom/$(echo $node).mr"

done

printf "
    engine
        .add_cylinder_bank(b0)

    engine.add_crankshaft(c0)

    harmonic_cam_lobe lobe(
        duration_at_50_thou: 256 * units.deg,
        gamma: 1.1,
        lift: 10.2 * units.mm,
        steps: 100
    )

    bmw_camshaft_builder camshaft(
	    lobe_profile: m52b28_lobe_profile_int(),
		ex_lobe_profile: m52b28_lobe_profile_exh()
	)

    b0.set_cylinder_head (
        bmw_m52b28_head(
            intake_camshaft: camshaft.intake_cam_0,
            exhaust_camshaft: camshaft.exhaust_cam_0
        )
    )

    function timing_curve(1000 * units.rpm)
    timing_curve
        .add_sample(1000 * units.rpm, 25 * units.deg)
        .add_sample(2000 * units.rpm, 25 * units.deg)
        .add_sample(3000 * units.rpm, 35 * units.deg)
        .add_sample(4000 * units.rpm, 35 * units.deg)
        .add_sample(5000 * units.rpm, 35 * units.deg)
        .add_sample(6000 * units.rpm, 45 * units.deg)
        .add_sample(7000 * units.rpm, 45 * units.deg)
        .add_sample(8000 * units.rpm, 50 * units.deg)

    engine.add_ignition_module(
        bmw_distributor(
            wires: wires,
            timing_curve: timing_curve,
            rev_limit: $rev_limit * units.rpm
        ))
}

\n" >> "../assets/engines/custom/$(echo $node).mr"

printf "
import \"engine_sim.mr\"
import \"themes/default.mr\"
import \"engines/custom/$node.mr\"

use_default_theme()
set_engine(
    $node()
)
\n" > "../assets/main.mr"

./engine-sim-app
