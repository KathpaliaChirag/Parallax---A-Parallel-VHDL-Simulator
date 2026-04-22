# CHIRAG 22-04-26 :: parallax dashboard
# single screen ... horizontal control bar at top
# clicking RUN runs BOTH seq and par simultaneously
# shows stats side by side + 3 charts below
#
# run from parallax root directory:
# streamlit run dashboard/app.py

import streamlit as st
import subprocess, re, os
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# -----------------------------------------------------------------------
# circuit registry
# -----------------------------------------------------------------------
CIRCUITS = {
    "and_gate":        {"name": "and gate",                   "vhdl": "tests/circuit/basic/and_gate.vhdl",        "tb": "",                                           "desc": "single 2-input AND gate",                   "procs": 1,   "hash": 2847733139},
    "multi_and":       {"name": "multi AND (4 gates)",         "vhdl": "tests/circuit/basic/multi_and.vhdl",       "tb": "tests/circuit/basic/multi_and_tb.txt",       "desc": "4 independent AND gates, no shared signals", "procs": 4,   "hash": 3198782091},
    "dff":             {"name": "D flip flop",                 "vhdl": "tests/circuit/basic/dff.vhdl",             "tb": "tests/circuit/basic/dff_tb.txt",             "desc": "captures D on rising clock edge",            "procs": 1,   "hash": 3702914041},
    "tff":             {"name": "T flip flop",                 "vhdl": "tests/circuit/basic/tff.vhdl",             "tb": "tests/circuit/basic/tff_tb.txt",             "desc": "toggles on clock when T=1",                 "procs": 2,   "hash": 3818917602},
    "srff":            {"name": "SR flip flop",                "vhdl": "tests/circuit/basic/srff.vhdl",            "tb": "tests/circuit/basic/srff_tb.txt",            "desc": "set/reset flip flop with clock",            "procs": 2,   "hash": 3798037844},
    "jkff":            {"name": "JK flip flop",                "vhdl": "tests/circuit/basic/jkff.vhdl",            "tb": "tests/circuit/basic/jkff_tb.txt",            "desc": "most versatile flip flop, J=K=1 toggles",  "procs": 2,   "hash": 2427382327},
    "ripple":          {"name": "ripple carry adder",          "vhdl": "tests/circuit/basic/ripple.vhdl",          "tb": "tests/circuit/basic/ripple_tb.txt",          "desc": "4-bit adder, carry ripples sequentially",   "procs": 4,   "hash": 342984039},
    "chainof4":        {"name": "chain of 4",                  "vhdl": "tests/circuit/basic/chainof4.vhdl",        "tb": "tests/circuit/basic/chainof4_tb.txt",        "desc": "4-stage pipeline, sequential dependencies", "procs": 4,   "hash": 749744446},
    "pipeline8":       {"name": "pipeline 8-stage",            "vhdl": "tests/circuit/basic/pipeline8.vhdl",       "tb": "tests/circuit/basic/pipeline8_tb.txt",       "desc": "8-stage fully sequential chain",            "procs": 8,   "hash": 3424237402},
    "eight_channel":   {"name": "8-channel OR bank",           "vhdl": "tests/circuit/basic/eight_channel.vhdl",   "tb": "tests/circuit/basic/eight_channel_tb.txt",   "desc": "8 independent OR gates",                    "procs": 8,   "hash": 844848091},
    "thirty2_channel": {"name": "32-channel OR bank",          "vhdl": "tests/circuit/basic/thirty2_channel.vhdl", "tb": "tests/circuit/basic/thirty2_channel_tb.txt", "desc": "32 independent OR gates, 5.9x speedup",    "procs": 32,  "hash": 1666589379},
    "wide_and128":     {"name": "wide AND 128",                "vhdl": "tests/circuit/basic/wide_and128.vhdl",     "tb": "tests/circuit/basic/wide_and128_tb.txt",     "desc": "128 independent AND gates, best for speedup","procs": 128, "hash": 1047282184},
    "traffic_light":   {"name": "traffic light FSM",           "vhdl": "tests/circuit/fsm/traffic_light.vhdl",     "tb": "tests/circuit/fsm/traffic_light_tb.txt",     "desc": "3-state FSM, sequential transitions",       "procs": 3,   "hash": 2957170177},
    "majority":        {"name": "majority voter",              "vhdl": "tests/circuit/basic/majority.vhdl",        "tb": "",                                           "desc": "5-input majority using VHDL function",      "procs": 1,   "hash": 1484108199},
    "hamming4":        {"name": "hamming encoder 4-bit",       "vhdl": "tests/circuit/basic/hamming4.vhdl",        "tb": "",                                           "desc": "3 independent parity processes",            "procs": 3,   "hash": 2071069720},
    "hamming32":       {"name": "hamming encoder 32-bit",      "vhdl": "tests/circuit/basic/hamming32.vhdl",       "tb": "tests/circuit/basic/hamming32_tb.txt",       "desc": "32 independent parity processes, 4.7x speedup","procs": 32, "hash": 4083200419},
    "cla4":            {"name": "carry lookahead adder 4-bit", "vhdl": "tests/circuit/basic/cla4.vhdl",            "tb": "tests/circuit/basic/cla4_tb.txt",            "desc": "G/P parallel, carry chain sequential",      "procs": 17,  "hash": 472018830},
    "cla32":           {"name": "carry lookahead adder 32-bit","vhdl": "tests/circuit/basic/cla32.vhdl",           "tb": "tests/circuit/basic/cla32_tb.txt",           "desc": "64 parallel G/P + 31 serial carry stages",  "procs": 97,  "hash": 3312919180},
}

# -----------------------------------------------------------------------
# run one simulation ... returns parsed stats dict + raw output
# -----------------------------------------------------------------------
def run_sim(circuit_key, mode, threads, stress):
    c = CIRCUITS[circuit_key]
    tb_arg = f"TB={c['tb']}" if c['tb'] else ""
    st_arg = f"STRESS={stress}" if stress > 0 else ""
    cmd = f"make circuit FILE={c['vhdl']} {tb_arg} MODE={mode} THREADS={threads} {st_arg}"
    try:
        r = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=180)
        out = r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        return {}, "timed out"
    s = {}
    for pat, key, typ in [
        (r"trace hash: (\d+)",                "hash",    int),
        (r"simulation time: ([\d.]+) seconds", "time",    float),
        (r"total delta cycles\s+: (\d+)",      "deltas",  int),
        (r"total events processed\s+: (\d+)",  "events",  int),
        (r"total process firings\s+: (\d+)",   "firings", int),
        (r"max delta depth\s+: (\d+)",         "maxd",    int),
        (r"num colors: (\d+)",                 "colors",  int),
        (r"num edges: (\d+)",                  "edges",   int),
        (r"num nodes: (\d+)",                  "nodes",   int),
    ]:
        m = re.search(pat, out)
        if m: s[key] = typ(m.group(1))
    return s, out

# -----------------------------------------------------------------------
# page setup
# -----------------------------------------------------------------------
st.set_page_config(page_title="Parallax", layout="wide", initial_sidebar_state="collapsed")

# CHIRAG 22-04-26 :: CSS
# going for a clean dark engineering aesthetic
# IBM Plex Mono for all numbers and labels ... feels precise
# single amber accent (#f59e0b) ... like an oscilloscope
# no gradients ... no glow ... no AI slop
AMBER = "#f59e0b"
st.markdown(f"""
<style>
@import url('https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:wght@400;500;600&family=IBM+Plex+Sans:wght@300;400;500&display=swap');

html, body, [class*="css"] {{
    font-family: 'IBM Plex Sans', sans-serif;
    background: #111;
    color: #ddd;
}}
#MainMenu, footer, header {{ visibility: hidden; }}
.stDeployButton {{ display: none; }}
section[data-testid="stSidebar"] {{ display: none; }}

/* top control bar */
.ctrl {{
    background: #1a1a1a;
    border: 1px solid #222;
    border-radius: 6px;
    padding: 1rem 1.5rem;
    margin-bottom: 1.2rem;
    display: flex;
    align-items: flex-end;
    gap: 1.5rem;
}}

/* run button */
.stButton > button {{
    background: {AMBER};
    color: #000;
    font-family: 'IBM Plex Mono', monospace;
    font-weight: 600;
    font-size: 0.78rem;
    letter-spacing: 0.06em;
    border: none;
    border-radius: 4px;
    padding: 0.55rem 1.4rem;
    width: 100%;
    white-space: nowrap;
}}
.stButton > button:hover {{ background: #d97706; color: #000; }}

/* stat panels */
.stat-panel {{
    background: #1a1a1a;
    border: 1px solid #252525;
    border-radius: 6px;
    padding: 1.2rem 1.4rem;
    height: 100%;
}}
.stat-panel.seq {{ border-top: 3px solid #555; }}
.stat-panel.par {{ border-top: 3px solid {AMBER}; }}
.panel-title {{
    font-family: 'IBM Plex Mono', monospace;
    font-size: 0.65rem;
    letter-spacing: 0.12em;
    text-transform: uppercase;
    color: #555;
    margin-bottom: 0.8rem;
}}
.big-number {{
    font-family: 'IBM Plex Mono', monospace;
    font-size: 2.2rem;
    font-weight: 600;
    color: #fff;
    line-height: 1;
}}
.big-unit {{
    font-family: 'IBM Plex Mono', monospace;
    font-size: 0.85rem;
    color: #555;
    margin-left: 0.3rem;
}}
.stat-row {{
    display: flex;
    justify-content: space-between;
    border-top: 1px solid #222;
    padding: 0.45rem 0;
    font-family: 'IBM Plex Mono', monospace;
    font-size: 0.78rem;
}}
.stat-row .label {{ color: #555; }}
.stat-row .value {{ color: #ccc; }}
.hash-ok  {{ color: #4ade80; font-size: 0.72rem; font-family: 'IBM Plex Mono', monospace; }}
.hash-bad {{ color: #f87171; font-size: 0.72rem; font-family: 'IBM Plex Mono', monospace; }}
.speedup-badge {{
    display: inline-block;
    background: #1f2a0d;
    border: 1px solid #365314;
    color: #a3e635;
    font-family: 'IBM Plex Mono', monospace;
    font-size: 0.9rem;
    font-weight: 600;
    padding: 0.2rem 0.6rem;
    border-radius: 3px;
    margin-top: 0.3rem;
}}
.speedup-bad {{
    background: #2d0808;
    border: 1px solid #7f1d1d;
    color: #f87171;
}}

/* section labels */
.section-label {{
    font-family: 'IBM Plex Mono', monospace;
    font-size: 0.62rem;
    letter-spacing: 0.14em;
    text-transform: uppercase;
    color: #444;
    margin: 1.2rem 0 0.5rem;
}}

/* footer */
.pf {{
    position: fixed; bottom: 0; left: 0; right: 0;
    background: #0d0d0d;
    border-top: 1px solid #1f1f1f;
    padding: 0.5rem 1.5rem;
    display: flex;
    justify-content: space-between;
    align-items: center;
    z-index: 9999;
    font-family: 'IBM Plex Mono', monospace;
}}
.pf-l {{ font-size: 0.7rem; color: {AMBER}; }}
.pf-r {{ font-size: 0.6rem; color: #3a3a3a; }}

.block-container {{
    padding: 1rem 2rem 5rem !important;
    max-width: 100% !important;
}}

/* selectbox, number input */
.stSelectbox > div > div,
.stNumberInput > div > div > input {{
    background: #1a1a1a !important;
    border: 1px solid #2a2a2a !important;
    color: #ddd !important;
    font-family: 'IBM Plex Mono', monospace !important;
    border-radius: 4px !important;
}}
label {{
    font-family: 'IBM Plex Mono', monospace !important;
    font-size: 0.65rem !important;
    letter-spacing: 0.1em !important;
    text-transform: uppercase !important;
    color: #555 !important;
}}
</style>
""", unsafe_allow_html=True)

PLOT = dict(
    paper_bgcolor="#1a1a1a",
    plot_bgcolor="#1a1a1a",
    font=dict(color="#999", family="IBM Plex Mono, monospace", size=11),
    margin=dict(l=50, r=20, t=35, b=45),
    xaxis=dict(gridcolor="#1f1f1f", linecolor="#2a2a2a", zerolinecolor="#2a2a2a"),
    yaxis=dict(gridcolor="#1f1f1f", linecolor="#2a2a2a", zerolinecolor="#2a2a2a"),
)

# -----------------------------------------------------------------------
# header
# -----------------------------------------------------------------------
st.markdown(
    '<p style="font-family:IBM Plex Mono,monospace;font-size:1.1rem;'
    'font-weight:600;color:#eee;margin-bottom:0.1rem;margin-top:0.2rem">'
    'Parallax &mdash; Parallel VHDL Simulator</p>'
    '<p style="font-family:IBM Plex Mono,monospace;font-size:0.65rem;'
    'color:#444;margin-top:0;margin-bottom:1rem">'
    'COD7001 &nbsp;/&nbsp; IIT Delhi &nbsp;/&nbsp; Chirag Kathpalia &nbsp;/&nbsp; 2025MCS2098</p>',
    unsafe_allow_html=True
)

# -----------------------------------------------------------------------
# horizontal control bar
# -----------------------------------------------------------------------
ck1, ck2, ck3, ck4, ck5 = st.columns([3, 1.2, 1, 1, 1])

with ck1:
    circuit_key = st.selectbox("Circuit",
        list(CIRCUITS.keys()),
        format_func=lambda k: f"{CIRCUITS[k]['name']}  ({CIRCUITS[k]['procs']} processes)",
        label_visibility="visible"
    )

with ck2:
    stress = st.number_input("Stress (flops/gate)", min_value=0,
                              max_value=2000000, value=0, step=1000)

with ck3:
    threads = st.select_slider("Par threads", [1, 2, 4, 8], value=4)

with ck4:
    st.markdown("<br>", unsafe_allow_html=True)
    run_btn = st.button("Run  seq + par")

with ck5:
    st.markdown("<br>", unsafe_allow_html=True)
    sweep_btn = st.button("Speedup sweep")

c = CIRCUITS[circuit_key]
st.markdown(
    f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.72rem;color:#555;margin:0 0 0.8rem">'
    f'{c["desc"]}</p>',
    unsafe_allow_html=True
)
st.divider()

# -----------------------------------------------------------------------
# RUN both seq and par
# -----------------------------------------------------------------------
if run_btn:
    with st.spinner("running sequential and parallel ..."):
        seq_s, seq_raw = run_sim(circuit_key, "--seq", 1,       stress)
        par_s, par_raw = run_sim(circuit_key, "--par", threads,  stress)

    st.session_state["seq_s"]   = seq_s
    st.session_state["par_s"]   = par_s
    st.session_state["seq_raw"] = seq_raw
    st.session_state["par_raw"] = par_raw
    st.session_state["threads"] = threads
    st.session_state["circuit"] = circuit_key
    st.session_state["sweep"]   = None

# -----------------------------------------------------------------------
# SPEEDUP SWEEP ... runs seq + par/1/2/4/8
# -----------------------------------------------------------------------
if sweep_btn:
    with st.spinner("running sweep ... seq + par 1/2/4/8 threads ..."):
        sweep = {}
        s0, _ = run_sim(circuit_key, "--seq", 1, stress)
        if s0 and "time" in s0:
            sweep["seq"] = s0
        for t in [1, 2, 4, 8]:
            sp, _ = run_sim(circuit_key, "--par", t, stress)
            if sp and "time" in sp:
                sweep[f"par_{t}"] = sp
    st.session_state["sweep"]   = sweep
    st.session_state["circuit"] = circuit_key
    st.session_state["threads"] = threads
    st.session_state["seq_s"]   = sweep.get("seq", {})
    st.session_state["par_s"]   = sweep.get(f"par_{threads}", {})

# -----------------------------------------------------------------------
# helper to render a stat panel
# -----------------------------------------------------------------------
def render_panel(label, stats, accent, threads_used=1, seq_time=None):
    if not stats or "time" not in stats:
        st.markdown(
            f'<div class="stat-panel"><div class="panel-title">{label}</div>'
            f'<p style="color:#444;font-family:IBM Plex Mono,monospace;font-size:0.8rem">no data yet</p></div>',
            unsafe_allow_html=True
        )
        return

    t = stats["time"]
    exp = CIRCUITS[st.session_state.get("circuit", circuit_key)].get("hash")
    got = stats.get("hash", 0)

    if exp and got == exp:
        hash_html = f'<span class="hash-ok">hash {got} verified</span>'
    elif exp:
        hash_html = f'<span class="hash-bad">hash mismatch  got {got}</span>'
    else:
        hash_html = f'<span style="color:#555;font-family:IBM Plex Mono,monospace;font-size:0.72rem">hash {got}</span>'

    speedup_html = ""
    if seq_time and seq_time > 0 and label != "sequential":
        sp = seq_time / t
        badge_cls = "speedup-badge" if sp >= 1 else "speedup-badge speedup-bad"
        speedup_html = f'<div class="{badge_cls}">{sp:.2f}x speedup</div>'

    rows = ""
    for lbl, val in [
        ("process firings", stats.get("firings", "--")),
        ("delta cycles",    stats.get("deltas",  "--")),
        ("events",          stats.get("events",  "--")),
        ("max delta depth", stats.get("maxd",    "--")),
        ("par batches",     stats.get("colors",  "--")),
        ("dependencies",    stats.get("edges",   "--")),
    ]:
        rows += f'<div class="stat-row"><span class="label">{lbl}</span><span class="value">{val}</span></div>'

    st.markdown(f"""
    <div class="stat-panel" style="border-top:3px solid {accent}">
        <div class="panel-title">{label}</div>
        <div class="big-number">{t:.4f}<span class="big-unit">s</span></div>
        {speedup_html}
        <div style="margin:0.7rem 0 0.3rem">{hash_html}</div>
        {rows}
    </div>
    """, unsafe_allow_html=True)

# -----------------------------------------------------------------------
# DISPLAY RESULTS
# -----------------------------------------------------------------------
if "seq_s" in st.session_state and st.session_state["seq_s"]:

    seq_s   = st.session_state["seq_s"]
    par_s   = st.session_state["par_s"]
    sweep   = st.session_state.get("sweep")
    th      = st.session_state.get("threads", threads)
    ckey    = st.session_state.get("circuit", circuit_key)
    seq_t   = seq_s.get("time", 0)
    par_t   = par_s.get("time", 0) if par_s else 0

    # stat panels side by side
    st.markdown('<div class="section-label">Simulation Results</div>', unsafe_allow_html=True)
    panel_l, panel_r = st.columns(2)
    with panel_l:
        render_panel("sequential", seq_s, "#555")
    with panel_r:
        render_panel(f"parallel  /  {th} threads", par_s, AMBER, th, seq_t)

    # charts
    st.markdown('<div class="section-label">Analysis</div>', unsafe_allow_html=True)
    ch1, ch2, ch3 = st.columns(3)

    # chart 1 ... seq vs par time comparison
    with ch1:
        fig1 = go.Figure()
        fig1.add_trace(go.Bar(
            name="sequential", x=["sequential"], y=[seq_t],
            marker_color="#3a3a3a",
            text=[f"{seq_t:.4f}s"], textposition="outside",
            textfont=dict(family="IBM Plex Mono", size=10, color="#999"),
        ))
        if par_t:
            fig1.add_trace(go.Bar(
                name=f"parallel ({th}T)", x=[f"parallel ({th}T)"], y=[par_t],
                marker_color=AMBER,
                text=[f"{par_t:.4f}s"], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#000"),
            ))
        fig1.update_layout(**PLOT, title="wall time (s)", showlegend=False,
                           height=280, yaxis_title="seconds")
        st.plotly_chart(fig1, use_container_width=True)

    # chart 2 ... overhead breakdown
    # overhead = par_time - (seq_time / threads) ... the part that didnt scale
    # useful   = seq_time / threads ... what each thread ideally does
    with ch2:
        if par_t and seq_t and th > 0:
            ideal_t   = seq_t / th
            overhead  = max(0, par_t - ideal_t)
            useful    = min(ideal_t, par_t)
            fig2 = go.Figure()
            fig2.add_trace(go.Bar(
                name="useful work",   x=["parallel"], y=[useful],
                marker_color="#22c55e",
                text=[f"{useful:.4f}s"], textposition="inside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#000"),
            ))
            fig2.add_trace(go.Bar(
                name="sync overhead", x=["parallel"], y=[overhead],
                marker_color="#ef4444",
                text=[f"{overhead:.4f}s"], textposition="inside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#fff"),
            ))
            fig2.add_trace(go.Bar(
                name="sequential",    x=["sequential"], y=[seq_t],
                marker_color="#3a3a3a",
            ))
            fig2.update_layout(**PLOT, title="time breakdown", barmode="stack",
                               height=280, yaxis_title="seconds",
                               legend=dict(font=dict(family="IBM Plex Mono", size=9),
                                           orientation="h", y=-0.25))
            st.plotly_chart(fig2, use_container_width=True)
        else:
            st.markdown('<p style="color:#444;font-family:IBM Plex Mono,monospace;font-size:0.8rem;margin-top:2rem">run simulation to see breakdown</p>', unsafe_allow_html=True)

    # chart 3 ... speedup sweep if available, else process firings comparison
    with ch3:
        if sweep and "seq" in sweep:
            st_time = sweep["seq"]["time"]
            xs, ys, colors = [], [], []
            for t in [1, 2, 4, 8]:
                key = f"par_{t}"
                if key in sweep:
                    sp = round(st_time / sweep[key]["time"], 2)
                    xs.append(str(t))
                    ys.append(sp)
                    colors.append(AMBER if sp >= 1 else "#ef4444")
            fig3 = go.Figure(go.Bar(
                x=xs, y=ys,
                marker_color=colors,
                text=[f"{y:.2f}x" for y in ys],
                textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=11, color="#fff"),
            ))
            fig3.add_hline(y=1.0, line_dash="dot", line_color="#444",
                           annotation_text="seq baseline",
                           annotation_font=dict(size=9, color="#555", family="IBM Plex Mono"))
            fig3.update_layout(**PLOT, title="speedup vs threads",
                               xaxis_title="threads", yaxis_title="speedup",
                               showlegend=False, height=280)
            st.plotly_chart(fig3, use_container_width=True)
        else:
            # show process firings + delta cycles comparison
            seq_f = seq_s.get("firings", 0)
            par_f = par_s.get("firings", 0) if par_s else 0
            seq_d = seq_s.get("deltas",  0)
            par_d = par_s.get("deltas",  0) if par_s else 0

            fig3 = go.Figure()
            fig3.add_trace(go.Bar(
                name="sequential", x=["firings", "delta cycles"],
                y=[seq_f, seq_d], marker_color="#3a3a3a",
            ))
            fig3.add_trace(go.Bar(
                name="parallel", x=["firings", "delta cycles"],
                y=[par_f, par_d], marker_color=AMBER,
            ))
            fig3.update_layout(**PLOT, title="simulation activity",
                               barmode="group", height=280,
                               legend=dict(font=dict(family="IBM Plex Mono", size=9),
                                           orientation="h", y=-0.25))
            st.plotly_chart(fig3, use_container_width=True)

    # dependency graph
    if os.path.exists("dependency.dot"):
        st.markdown('<div class="section-label">Dependency Graph</div>', unsafe_allow_html=True)
        with open("dependency.dot") as f:
            st.graphviz_chart(f.read(), use_container_width=False)

else:
    # default state ... show known benchmarks as a warm welcome
    st.markdown('<div class="section-label">Known Benchmarks &nbsp; / &nbsp; stress=100000 &nbsp; / &nbsp; 8 threads &nbsp; / &nbsp; WSL2</div>', unsafe_allow_html=True)

    b1, b2 = st.columns([2, 1])
    with b1:
        import pandas as pd
        df = pd.DataFrame([
            {"circuit": "thirty2_channel", "processes": 32,  "parallel batches": 1,  "speedup / 8T": "5.90x", "bottleneck": "none ... fully parallel"},
            {"circuit": "hamming32",       "processes": 32,  "parallel batches": 1,  "speedup / 8T": "4.71x", "bottleneck": "none ... fully parallel"},
            {"circuit": "wide_and128",     "processes": 128, "parallel batches": 1,  "speedup / 8T": "3.93x", "bottleneck": "none ... fully parallel"},
            {"circuit": "cla32",           "processes": 97,  "parallel batches": 33, "speedup / 8T": "1.75x", "bottleneck": "31-stage carry chain"},
            {"circuit": "pipeline8",       "processes": 8,   "parallel batches": 8,  "speedup / 8T": "< 1x",  "bottleneck": "fully sequential"},
        ])
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.markdown(
            '<p style="font-family:IBM Plex Mono,monospace;font-size:0.72rem;color:#444;margin-top:0.6rem">'
            'parallel batches (colors) is the key metric. 1 batch = all processes independent = scales with threads. '
            'many batches = sequential dependency chain = Amdahl limit.</p>',
            unsafe_allow_html=True
        )

    with b2:
        if os.path.exists("dependency.dot"):
            st.markdown('<div class="section-label">Last Dependency Graph</div>', unsafe_allow_html=True)
            with open("dependency.dot") as f:
                st.graphviz_chart(f.read(), use_container_width=True)

# -----------------------------------------------------------------------
# footer
# -----------------------------------------------------------------------
st.markdown("""
<div class="pf">
    <span class="pf-l">sequential first. parallel second. correctness always.</span>
    <span class="pf-r">Chirag Kathpalia / 2025MCS2098 / COD7001 IIT Delhi / built with AI assistance (Claude, Anthropic)</span>
</div>
""", unsafe_allow_html=True)