# CHIRAG 22-04-26 :: parallax dashboard v3
# fixed ... no HTML in panels ... native streamlit components throughout
# sweep shows speedup chart across 1/2/4/8 threads
# dependency graph hidden behind toggle button
# gtkwave button next to dependency toggle
#
# run from parallax root: streamlit run dashboard/app.py

import streamlit as st
import subprocess, re, os
import plotly.graph_objects as go

AMBER = "#f59e0b"

CIRCUITS = {
    "and_gate":        {"name": "and gate",                   "vhdl": "tests/circuit/basic/and_gate.vhdl",        "tb": "",                                           "desc": "single 2-input AND gate",                    "procs": 1,   "hash": 2847733139},
    "multi_and":       {"name": "multi AND (4 gates)",         "vhdl": "tests/circuit/basic/multi_and.vhdl",       "tb": "tests/circuit/basic/multi_and_tb.txt",       "desc": "4 independent AND gates",                    "procs": 4,   "hash": 3198782091},
    "dff":             {"name": "D flip flop",                 "vhdl": "tests/circuit/basic/dff.vhdl",             "tb": "tests/circuit/basic/dff_tb.txt",             "desc": "captures D on rising clock edge",            "procs": 1,   "hash": 3702914041},
    "tff":             {"name": "T flip flop",                 "vhdl": "tests/circuit/basic/tff.vhdl",             "tb": "tests/circuit/basic/tff_tb.txt",             "desc": "toggles on clock when T=1",                 "procs": 2,   "hash": 3818917602},
    "srff":            {"name": "SR flip flop",                "vhdl": "tests/circuit/basic/srff.vhdl",            "tb": "tests/circuit/basic/srff_tb.txt",            "desc": "set/reset flip flop with clock",            "procs": 2,   "hash": 3798037844},
    "jkff":            {"name": "JK flip flop",                "vhdl": "tests/circuit/basic/jkff.vhdl",            "tb": "tests/circuit/basic/jkff_tb.txt",            "desc": "most versatile flip flop",                  "procs": 2,   "hash": 2427382327},
    "ripple":          {"name": "ripple carry adder",          "vhdl": "tests/circuit/basic/ripple.vhdl",          "tb": "tests/circuit/basic/ripple_tb.txt",          "desc": "4-bit adder, carry ripples sequentially",   "procs": 4,   "hash": 342984039},
    "chainof4":        {"name": "chain of 4",                  "vhdl": "tests/circuit/basic/chainof4.vhdl",        "tb": "tests/circuit/basic/chainof4_tb.txt",        "desc": "4-stage sequential pipeline",               "procs": 4,   "hash": 749744446},
    "pipeline8":       {"name": "pipeline 8-stage",            "vhdl": "tests/circuit/basic/pipeline8.vhdl",       "tb": "tests/circuit/basic/pipeline8_tb.txt",       "desc": "8-stage fully sequential chain",            "procs": 8,   "hash": 3424237402},
    "eight_channel":   {"name": "8-channel OR bank",           "vhdl": "tests/circuit/basic/eight_channel.vhdl",   "tb": "tests/circuit/basic/eight_channel_tb.txt",   "desc": "8 independent OR gates",                    "procs": 8,   "hash": 844848091},
    "thirty2_channel": {"name": "32-channel OR bank",          "vhdl": "tests/circuit/basic/thirty2_channel.vhdl", "tb": "tests/circuit/basic/thirty2_channel_tb.txt", "desc": "32 independent OR gates, 5.9x speedup",    "procs": 32,  "hash": 1666589379},
    "wide_and128":     {"name": "wide AND 128",                "vhdl": "tests/circuit/basic/wide_and128.vhdl",     "tb": "tests/circuit/basic/wide_and128_tb.txt",     "desc": "128 independent AND gates, best speedup",   "procs": 128, "hash": 1047282184},
    "traffic_light":   {"name": "traffic light FSM",           "vhdl": "tests/circuit/fsm/traffic_light.vhdl",     "tb": "tests/circuit/fsm/traffic_light_tb.txt",     "desc": "3-state FSM, sequential transitions",       "procs": 3,   "hash": 2957170177},
    "majority":        {"name": "majority voter",              "vhdl": "tests/circuit/basic/majority.vhdl",        "tb": "",                                           "desc": "5-input majority using VHDL function",      "procs": 1,   "hash": 1484108199},
    "hamming4":        {"name": "hamming encoder 4-bit",       "vhdl": "tests/circuit/basic/hamming4.vhdl",        "tb": "",                                           "desc": "3 independent parity processes",            "procs": 3,   "hash": 2071069720},
    "hamming32":       {"name": "hamming encoder 32-bit",      "vhdl": "tests/circuit/basic/hamming32.vhdl",       "tb": "tests/circuit/basic/hamming32_tb.txt",       "desc": "32 independent parity processes, 4.7x",     "procs": 32,  "hash": 4083200419},
    "cla4":            {"name": "carry lookahead adder 4-bit", "vhdl": "tests/circuit/basic/cla4.vhdl",            "tb": "tests/circuit/basic/cla4_tb.txt",            "desc": "G/P parallel, carry chain sequential",      "procs": 17,  "hash": 472018830},
    "cla32":           {"name": "carry lookahead adder 32-bit","vhdl": "tests/circuit/basic/cla32.vhdl",           "tb": "tests/circuit/basic/cla32_tb.txt",           "desc": "64 parallel G/P + 31 serial carry stages",  "procs": 97,  "hash": 3312919180},
}

def run_sim(circuit_key, mode, threads, stress):
    c = CIRCUITS[circuit_key]
    tb = f"TB={c['tb']}" if c['tb'] else ""
    st_arg = f"STRESS={stress}" if stress > 0 else ""
    cmd = f"make circuit FILE={c['vhdl']} {tb} MODE={mode} THREADS={threads} {st_arg}"
    try:
        r = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=200)
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
st.set_page_config(page_title="Parallax", layout="wide", initial_sidebar_state="collapsed")

st.markdown(f"""
<style>
@import url('https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:wght@400;500;600&family=IBM+Plex+Sans:wght@300;400;500&display=swap');

html, body, [class*="css"] {{ font-family:'IBM Plex Sans',sans-serif; background:#111; color:#ddd; }}
#MainMenu, footer, header {{ visibility:hidden; }}
.stDeployButton {{ display:none; }}
section[data-testid="stSidebar"] {{ display:none; }}
.block-container {{ padding:1rem 2rem 5rem !important; max-width:100% !important; }}

/* control bar */
.stSelectbox > div > div,
.stNumberInput > div > div > input {{
    background:#1a1a1a !important; border:1px solid #2a2a2a !important;
    color:#ddd !important; font-family:'IBM Plex Mono',monospace !important; border-radius:4px !important;
}}
label {{
    font-family:'IBM Plex Mono',monospace !important; font-size:0.62rem !important;
    letter-spacing:0.1em !important; text-transform:uppercase !important; color:#555 !important;
}}

/* buttons */
.stButton > button {{
    background:{AMBER}; color:#000; font-family:'IBM Plex Mono',monospace;
    font-weight:600; font-size:0.75rem; letter-spacing:0.05em;
    border:none; border-radius:4px; padding:0.55rem 1.2rem; width:100%;
}}
.stButton > button:hover {{ background:#d97706; color:#000; }}

/* stat panels */
[data-testid="metric-container"] {{
    background:#191919; border:1px solid #222; border-radius:4px; padding:0.7rem 1rem;
}}
[data-testid="stMetricValue"] {{ font-family:'IBM Plex Mono',monospace !important; color:#eee !important; }}
[data-testid="stMetricLabel"] {{ font-family:'IBM Plex Mono',monospace !important; color:#555 !important; }}
[data-testid="stMetricDelta"] {{ font-family:'IBM Plex Mono',monospace !important; }}

/* divider */
hr {{ border-color:#222 !important; margin:0.8rem 0 !important; }}

/* section labels */
.slabel {{
    font-family:'IBM Plex Mono',monospace; font-size:0.6rem;
    letter-spacing:0.14em; text-transform:uppercase; color:#444; margin:1rem 0 0.5rem;
}}

/* big time display */
.bigtime {{
    font-family:'IBM Plex Mono',monospace; font-size:2.4rem; font-weight:600;
    color:#fff; line-height:1; margin-bottom:0.3rem;
}}
.bigtime .unit {{ font-size:0.9rem; color:#555; margin-left:0.2rem; }}

/* panel wrapper */
.panel {{
    background:#191919; border:1px solid #252525;
    border-radius:6px; padding:1.2rem 1.4rem; height:100%;
}}
.panel-seq {{ border-top:3px solid #3a3a3a; }}
.panel-par {{ border-top:3px solid {AMBER}; }}
.panel-title {{ font-family:'IBM Plex Mono',monospace; font-size:0.62rem; letter-spacing:0.12em; text-transform:uppercase; color:#555; margin-bottom:0.8rem; }}

/* speedup badge */
.spbadge {{ display:inline-block; background:#1a2e0d; border:1px solid #365314; color:#86efac; font-family:'IBM Plex Mono',monospace; font-size:0.85rem; font-weight:600; padding:0.15rem 0.6rem; border-radius:3px; margin:0.3rem 0 0.5rem; }}
.spbadge-bad {{ background:#2d0a0a; border-color:#7f1d1d; color:#fca5a5; }}

/* hash line */
.hok {{ font-family:'IBM Plex Mono',monospace; font-size:0.7rem; color:#4ade80; }}
.hbad {{ font-family:'IBM Plex Mono',monospace; font-size:0.7rem; color:#f87171; }}

/* stat row inside panel */
.srow {{ display:flex; justify-content:space-between; border-top:1px solid #222; padding:0.38rem 0; font-family:'IBM Plex Mono',monospace; font-size:0.75rem; }}
.srow .lbl {{ color:#555; }} .srow .val {{ color:#bbb; }}

/* footer */
.pf {{ position:fixed; bottom:0; left:0; right:0; background:#0d0d0d; border-top:1px solid #1e1e1e;
    padding:0.45rem 1.5rem; display:flex; justify-content:space-between; align-items:center; z-index:9999;
    font-family:'IBM Plex Mono',monospace; }}
.pfl {{ font-size:0.68rem; color:{AMBER}; }}
.pfr {{ font-size:0.58rem; color:#333; }}
</style>
""", unsafe_allow_html=True)

PLOT = dict(
    paper_bgcolor="#191919", plot_bgcolor="#191919",
    font=dict(color="#888", family="IBM Plex Mono, monospace", size=11),
    margin=dict(l=45, r=15, t=30, b=40),
    xaxis=dict(gridcolor="#1e1e1e", linecolor="#252525", zerolinecolor="#252525"),
    yaxis=dict(gridcolor="#1e1e1e", linecolor="#252525", zerolinecolor="#252525"),
)

# -----------------------------------------------------------------------
# header
# -----------------------------------------------------------------------
st.markdown(
    '<p style="font-family:IBM Plex Mono,monospace;font-size:1.05rem;font-weight:600;color:#eee;margin-bottom:0.05rem;margin-top:0.2rem">Parallax — Parallel VHDL Simulator</p>'
    '<p style="font-family:IBM Plex Mono,monospace;font-size:0.62rem;color:#3a3a3a;margin-top:0;margin-bottom:0.8rem">COD7001 &nbsp;/&nbsp; IIT Delhi &nbsp;/&nbsp; Chirag Kathpalia &nbsp;/&nbsp; 2025MCS2098</p>',
    unsafe_allow_html=True
)

# -----------------------------------------------------------------------
# control bar
# -----------------------------------------------------------------------
c1, c2, c3, c4, c5 = st.columns([3.5, 1.2, 0.9, 0.9, 0.9])
with c1:
    circuit_key = st.selectbox("Circuit", list(CIRCUITS.keys()),
        format_func=lambda k: f"{CIRCUITS[k]['name']}  ({CIRCUITS[k]['procs']} processes)")
with c2:
    stress = st.number_input("Stress (flops/gate)", min_value=0, max_value=2000000, value=0, step=1000)
with c3:
    threads = st.select_slider("Par threads", [1,2,4,8], value=4)
with c4:
    st.write("")
    run_btn = st.button("Run seq + par")
with c5:
    st.write("")
    sweep_btn = st.button("Speedup sweep")

c = CIRCUITS[circuit_key]
st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.7rem;color:#444;margin:0.1rem 0 0.5rem">{c["desc"]}</p>', unsafe_allow_html=True)
st.divider()

# -----------------------------------------------------------------------
# helper ... draw one stat panel using only st.markdown (the HTML works for panels but NOT for metrics)
# the bug was render_panel was called inside columns ... st.markdown HTML rendering in columns
# is inconsistent in streamlit ... fix is to write ALL content as one big html block
# -----------------------------------------------------------------------
def stat_panel(label, stats, panel_cls, seq_time=None, thread_count=1):
    if not stats or "time" not in stats:
        st.markdown(f'<div class="panel {panel_cls}"><div class="panel-title">{label}</div><p style="color:#333;font-family:IBM Plex Mono,monospace;font-size:0.8rem">no data</p></div>', unsafe_allow_html=True)
        return

    t = stats["time"]
    exp = c.get("hash")
    got = stats.get("hash", 0)
    hash_html = f'<span class="hok">hash {got} verified</span>' if (exp and got == exp) else f'<span class="hbad">hash mismatch  got {got}</span>' if exp else f'<span style="color:#444;font-family:IBM Plex Mono,monospace;font-size:0.7rem">hash {got}</span>'

    badge = ""
    if seq_time and seq_time > 0 and "seq" not in panel_cls.lower() and label != "sequential":
        sp = seq_time / t
        cls = "spbadge" if sp >= 1 else "spbadge spbadge-bad"
        badge = f'<div class="{cls}">{sp:.2f}x speedup / {thread_count} threads</div>'

    rows = "".join([
        f'<div class="srow"><span class="lbl">{lbl}</span><span class="val">{stats.get(k, "--")}</span></div>'
        for lbl, k in [
            ("process firings", "firings"),
            ("delta cycles",    "deltas"),
            ("events",          "events"),
            ("max delta depth", "maxd"),
            ("par batches",     "colors"),
            ("dependencies",    "edges"),
        ]
    ])

    st.markdown(f"""
    <div class="panel {panel_cls}">
        <div class="panel-title">{label}</div>
        <div class="bigtime">{t:.4f}<span class="unit">s</span></div>
        {badge}
        <div style="margin:0.4rem 0 0.5rem">{hash_html}</div>
        {rows}
    </div>""", unsafe_allow_html=True)

# -----------------------------------------------------------------------
# RUN
# -----------------------------------------------------------------------
if run_btn:
    with st.spinner("running sequential and parallel ..."):
        ss, sr = run_sim(circuit_key, "--seq", 1,       stress)
        ps, pr = run_sim(circuit_key, "--par", threads,  stress)
    st.session_state.update({"ss":ss,"ps":ps,"sr":sr,"pr":pr,"sweep":None,"ck":circuit_key,"th":threads})

if sweep_btn:
    with st.spinner("running sweep ... seq + par 1 / 2 / 4 / 8 threads ..."):
        sweep = {}
        s0, _ = run_sim(circuit_key, "--seq", 1, stress)
        if s0 and "time" in s0: sweep["seq"] = s0
        for t in [1,2,4,8]:
            sp, _ = run_sim(circuit_key, "--par", t, stress)
            if sp and "time" in sp: sweep[f"par_{t}"] = sp
    st.session_state.update({"sweep":sweep,"ck":circuit_key,"th":threads,
                              "ss":sweep.get("seq",{}),"ps":sweep.get(f"par_{threads}",{})})

# -----------------------------------------------------------------------
# display
# -----------------------------------------------------------------------
if "ss" in st.session_state and st.session_state["ss"]:
    ss   = st.session_state["ss"]
    ps   = st.session_state["ps"]
    sweep= st.session_state.get("sweep")
    th   = st.session_state.get("th", threads)
    ck   = st.session_state.get("ck", circuit_key)
    c    = CIRCUITS[ck]
    seq_t= ss.get("time", 0)
    par_t= ps.get("time", 0) if ps else 0

    # ---- charts first (moved up) ----
    st.markdown('<div class="slabel">Analysis</div>', unsafe_allow_html=True)
    ch1, ch2, ch3 = st.columns(3)

    with ch1:
        fig1 = go.Figure()
        fig1.add_trace(go.Bar(x=["seq"], y=[seq_t], name="seq",
            marker_color="#3a3a3a", text=[f"{seq_t:.4f}s"], textposition="outside",
            textfont=dict(family="IBM Plex Mono", size=10, color="#888")))
        if par_t:
            fig1.add_trace(go.Bar(x=[f"par ({th}T)"], y=[par_t], name=f"par ({th}T)",
                marker_color=AMBER, text=[f"{par_t:.4f}s"], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#000")))
        fig1.update_layout(**PLOT, title="wall time (s)", showlegend=False,
                           yaxis_title="seconds", height=260)
        st.plotly_chart(fig1, use_container_width=True)

    with ch2:
        if par_t and seq_t and th > 0:
            ideal   = seq_t / th
            overhead= max(0, par_t - ideal)
            useful  = min(ideal, par_t)
            fig2 = go.Figure()
            fig2.add_trace(go.Bar(x=["par"], y=[useful],   name="useful work",   marker_color="#22c55e"))
            fig2.add_trace(go.Bar(x=["par"], y=[overhead], name="sync overhead", marker_color="#ef4444"))
            fig2.add_trace(go.Bar(x=["seq"], y=[seq_t],    name="sequential",    marker_color="#3a3a3a"))
            fig2.update_layout(**PLOT, title="time breakdown", barmode="stack",
                               yaxis_title="seconds", height=260,
                               legend=dict(font=dict(family="IBM Plex Mono", size=9), orientation="h", y=-0.3))
            st.plotly_chart(fig2, use_container_width=True)

    with ch3:
        if sweep and "seq" in sweep:
            st_time = sweep["seq"]["time"]
            xs, ys, cols = [], [], []
            for t in [1,2,4,8]:
                key = f"par_{t}"
                if key in sweep:
                    sp = round(st_time / sweep[key]["time"], 2)
                    xs.append(str(t)); ys.append(sp)
                    cols.append(AMBER if sp >= 1 else "#ef4444")
            fig3 = go.Figure(go.Bar(x=xs, y=ys, marker_color=cols,
                text=[f"{y:.2f}x" for y in ys], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=11, color="#fff")))
            fig3.add_hline(y=1.0, line_dash="dot", line_color="#444",
                annotation_text="seq baseline",
                annotation_font=dict(size=9, color="#555", family="IBM Plex Mono"))
            fig3.update_layout(**PLOT, title="speedup vs threads",
                xaxis_title="threads", yaxis_title="speedup", showlegend=False, height=260)
            st.plotly_chart(fig3, use_container_width=True)
        else:
            # placeholder ... firings comparison
            seq_f = ss.get("firings", 0); par_f = ps.get("firings", 0) if ps else 0
            fig3 = go.Figure()
            fig3.add_trace(go.Bar(name="seq", x=["firings","deltas"],
                y=[ss.get("firings",0), ss.get("deltas",0)], marker_color="#3a3a3a"))
            fig3.add_trace(go.Bar(name="par", x=["firings","deltas"],
                y=[ps.get("firings",0) if ps else 0, ps.get("deltas",0) if ps else 0],
                marker_color=AMBER))
            fig3.update_layout(**PLOT, title="simulation activity", barmode="group",
                height=260, legend=dict(font=dict(family="IBM Plex Mono", size=9),
                orientation="h", y=-0.3))
            st.plotly_chart(fig3, use_container_width=True)

    # ---- stat panels below charts ----
    st.markdown('<div class="slabel">Simulation Stats</div>', unsafe_allow_html=True)
    pl, pr_col = st.columns(2)
    with pl:
        stat_panel("sequential", ss, "panel-seq")
    with pr_col:
        stat_panel(f"parallel  /  {th} threads", ps, "panel-par", seq_t, th)

    # ---- dependency graph + gtkwave toggle ----
    st.markdown('<div class="slabel" style="margin-top:1.2rem"></div>', unsafe_allow_html=True)
    btn1, btn2, _ = st.columns([1.2, 1.2, 6])
    with btn1:
        show_graph = st.button("Show dependency graph")
    with btn2:
        # gtkwave ... find VCD file for this circuit
        vcd_name = f"output-{ck}.vcd"
        if st.button("Open in GTKWave"):
            if os.path.exists(vcd_name):
                subprocess.Popen(["gtkwave", vcd_name])
                st.success(f"opened {vcd_name} in GTKWave")
            else:
                st.warning(f"{vcd_name} not found ... run simulation first")

    if show_graph:
        if os.path.exists("dependency.dot"):
            with open("dependency.dot") as f:
                st.graphviz_chart(f.read(), use_container_width=False)
        else:
            st.info("no dependency.dot ... run a simulation first")

else:
    # ---- default state ----
    import pandas as pd
    st.markdown('<div class="slabel">Known Benchmarks &nbsp;/&nbsp; stress=100000 &nbsp;/&nbsp; 8 threads &nbsp;/&nbsp; WSL2</div>', unsafe_allow_html=True)
    dl, dr = st.columns([2, 1])
    with dl:
        df = pd.DataFrame([
            {"circuit": "thirty2_channel", "processes": 32,  "par batches": 1,  "speedup/8T": "5.90x", "note": "all independent, fewest delta cycles"},
            {"circuit": "hamming32",       "processes": 32,  "par batches": 1,  "speedup/8T": "4.71x", "note": "32 fully independent parity processes"},
            {"circuit": "wide_and128",     "processes": 128, "par batches": 1,  "speedup/8T": "3.93x", "note": "128 independent AND gates"},
            {"circuit": "cla32",           "processes": 97,  "par batches": 33, "speedup/8T": "1.75x", "note": "31-stage carry chain limits speedup"},
            {"circuit": "pipeline8",       "processes": 8,   "par batches": 8,  "speedup/8T": "< 1x",  "note": "fully sequential, no parallelism"},
        ])
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.7rem;color:#3a3a3a;margin-top:0.5rem">1 parallel batch = all independent = scales with threads. many batches = Amdahl limit.</p>', unsafe_allow_html=True)
    with dr:
        if os.path.exists("dependency.dot"):
            st.markdown('<div class="slabel">Last Dependency Graph</div>', unsafe_allow_html=True)
            with open("dependency.dot") as f:
                st.graphviz_chart(f.read(), use_container_width=True)

# footer
st.markdown('<div class="pf"><span class="pfl">sequential first. parallel second. correctness always.</span><span class="pfr">Chirag Kathpalia / 2025MCS2098 / COD7001 IIT Delhi / built with AI assistance (Claude, Anthropic)</span></div>', unsafe_allow_html=True)