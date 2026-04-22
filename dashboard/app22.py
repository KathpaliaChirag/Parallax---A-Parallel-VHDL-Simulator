# CHIRAG 22-04-26 :: parallax dashboard v4
# root cause of left panel bug ... css classes inside st.columns dont apply consistently
# fix ... ZERO custom css classes in panel content ... inline styles only OR native components
# everything in panels is now native streamlit ... no st.markdown with html in panels
#
# run from parallax root: streamlit run dashboard/app.py

import streamlit as st
import subprocess, re, os, multiprocessing
import plotly.graph_objects as go

AMBER  = "#f59e0b"
GREEN  = "#22c55e"
RED    = "#ef4444"
DIM    = "#3a3a3a"
BG     = "#191919"

CIRCUITS = {
    "and_gate":        {"name":"and gate",                   "vhdl":"tests/circuit/basic/and_gate.vhdl",        "tb":"",                                           "desc":"single 2-input AND gate",                    "procs":1,   "hash":2847733139},
    "multi_and":       {"name":"multi AND (4 gates)",         "vhdl":"tests/circuit/basic/multi_and.vhdl",       "tb":"tests/circuit/basic/multi_and_tb.txt",       "desc":"4 independent AND gates, no shared signals", "procs":4,   "hash":3198782091},
    "dff":             {"name":"D flip flop",                 "vhdl":"tests/circuit/basic/dff.vhdl",             "tb":"tests/circuit/basic/dff_tb.txt",             "desc":"captures D on rising clock edge",            "procs":1,   "hash":3702914041},
    "tff":             {"name":"T flip flop",                 "vhdl":"tests/circuit/basic/tff.vhdl",             "tb":"tests/circuit/basic/tff_tb.txt",             "desc":"toggles on clock when T=1",                 "procs":2,   "hash":3818917602},
    "srff":            {"name":"SR flip flop",                "vhdl":"tests/circuit/basic/srff.vhdl",            "tb":"tests/circuit/basic/srff_tb.txt",            "desc":"set/reset flip flop with clock",            "procs":2,   "hash":3798037844},
    "jkff":            {"name":"JK flip flop",                "vhdl":"tests/circuit/basic/jkff.vhdl",            "tb":"tests/circuit/basic/jkff_tb.txt",            "desc":"most versatile flip flop",                  "procs":2,   "hash":2427382327},
    "ripple":          {"name":"ripple carry adder",          "vhdl":"tests/circuit/basic/ripple.vhdl",          "tb":"tests/circuit/basic/ripple_tb.txt",          "desc":"4-bit adder, carry ripples sequentially",   "procs":4,   "hash":342984039},
    "chainof4":        {"name":"chain of 4",                  "vhdl":"tests/circuit/basic/chainof4.vhdl",        "tb":"tests/circuit/basic/chainof4_tb.txt",        "desc":"4-stage sequential pipeline",               "procs":4,   "hash":749744446},
    "pipeline8":       {"name":"pipeline 8-stage",            "vhdl":"tests/circuit/basic/pipeline8.vhdl",       "tb":"tests/circuit/basic/pipeline8_tb.txt",       "desc":"8-stage fully sequential chain",            "procs":8,   "hash":3424237402},
    "eight_channel":   {"name":"8-channel OR bank",           "vhdl":"tests/circuit/basic/eight_channel.vhdl",   "tb":"tests/circuit/basic/eight_channel_tb.txt",   "desc":"8 independent OR gates",                    "procs":8,   "hash":844848091},
    "thirty2_channel": {"name":"32-channel OR bank",          "vhdl":"tests/circuit/basic/thirty2_channel.vhdl", "tb":"tests/circuit/basic/thirty2_channel_tb.txt", "desc":"32 independent OR gates, 5.9x speedup",    "procs":32,  "hash":1666589379},
    "wide_and128":     {"name":"wide AND 128",                "vhdl":"tests/circuit/basic/wide_and128.vhdl",     "tb":"tests/circuit/basic/wide_and128_tb.txt",     "desc":"128 independent AND gates, best speedup",   "procs":128, "hash":1047282184},
    "traffic_light":   {"name":"traffic light FSM",           "vhdl":"tests/circuit/fsm/traffic_light.vhdl",     "tb":"tests/circuit/fsm/traffic_light_tb.txt",     "desc":"3-state FSM, sequential transitions",       "procs":3,   "hash":2957170177},
    "majority":        {"name":"majority voter",              "vhdl":"tests/circuit/basic/majority.vhdl",        "tb":"",                                           "desc":"5-input majority using VHDL function",      "procs":1,   "hash":1484108199},
    "hamming4":        {"name":"hamming encoder 4-bit",       "vhdl":"tests/circuit/basic/hamming4.vhdl",        "tb":"",                                           "desc":"3 independent parity processes",            "procs":3,   "hash":2071069720},
    "hamming32":       {"name":"hamming encoder 32-bit",      "vhdl":"tests/circuit/basic/hamming32.vhdl",       "tb":"tests/circuit/basic/hamming32_tb.txt",       "desc":"32 independent parity processes, 4.7x",     "procs":32,  "hash":4083200419},
    "cla4":            {"name":"carry lookahead adder 4-bit", "vhdl":"tests/circuit/basic/cla4.vhdl",            "tb":"tests/circuit/basic/cla4_tb.txt",            "desc":"G/P parallel, carry chain sequential",      "procs":17,  "hash":472018830},
    "cla32":           {"name":"carry lookahead adder 32-bit","vhdl":"tests/circuit/basic/cla32.vhdl",           "tb":"tests/circuit/basic/cla32_tb.txt",           "desc":"64 parallel G/P + 31 serial carry stages",  "procs":97,  "hash":3312919180},
}

def run_sim(circuit_key, mode, threads, stress):
    c = CIRCUITS[circuit_key]
    tb = f"TB={c['tb']}" if c['tb'] else ""
    sa = f"STRESS={stress}" if stress > 0 else ""
    cmd = f"make circuit FILE={c['vhdl']} {tb} MODE={mode} THREADS={threads} {sa}"
    try:
        r = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=240)
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

# auto-detect logical CPU count
CPU_COUNT   = multiprocessing.cpu_count()
THREAD_OPTS = [t for t in [1,2,4,8,16,32] if t <= CPU_COUNT]
if not THREAD_OPTS: THREAD_OPTS = [1]
DEFAULT_TH  = min(8, CPU_COUNT)

# -----------------------------------------------------------------------
st.set_page_config(page_title="Parallax", layout="wide", initial_sidebar_state="collapsed")

st.markdown(f"""
<style>
@import url('https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:wght@400;500;600&family=IBM+Plex+Sans:wght@300;400;500&display=swap');

html, body, [class*="css"] {{ font-family:'IBM Plex Sans',sans-serif; background:#111; color:#ddd; }}
#MainMenu, footer, header {{ visibility:hidden; }}
.stDeployButton {{ display:none; }}
section[data-testid="stSidebar"] {{ display:none; }}
.block-container {{ padding:0.8rem 1.8rem 5rem !important; max-width:100% !important; }}

.stSelectbox > div > div {{
    background:#1a1a1a !important; border:1px solid #2a2a2a !important;
    color:#ddd !important; font-family:'IBM Plex Mono',monospace !important; border-radius:4px !important;
}}
.stNumberInput > div > div > input {{
    background:#1a1a1a !important; border:1px solid #2a2a2a !important;
    color:#ddd !important; font-family:'IBM Plex Mono',monospace !important;
}}
label, .stSlider label {{
    font-family:'IBM Plex Mono',monospace !important; font-size:0.62rem !important;
    letter-spacing:0.1em !important; text-transform:uppercase !important; color:#555 !important;
}}
.stButton > button {{
    background:{AMBER}; color:#000; font-family:'IBM Plex Mono',monospace;
    font-weight:600; font-size:0.74rem; letter-spacing:0.05em;
    border:none; border-radius:4px; padding:0.5rem 1rem; width:100%;
}}
.stButton > button:hover {{ background:#d97706; color:#000; }}
[data-testid="metric-container"] {{
    background:#181818; border:1px solid #242424; border-radius:4px; padding:0.6rem 0.9rem;
}}
[data-testid="stMetricValue"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:1.1rem !important; color:#eee !important; }}
[data-testid="stMetricLabel"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:0.6rem !important; color:#555 !important; letter-spacing:0.1em !important; text-transform:uppercase !important; }}
[data-testid="stMetricDelta"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:0.75rem !important; }}
hr {{ border-color:#1e1e1e !important; margin:0.7rem 0 !important; }}
.pf {{ position:fixed; bottom:0; left:0; right:0; background:#0d0d0d; border-top:1px solid #1a1a1a;
    padding:0.4rem 1.5rem; display:flex; justify-content:space-between; align-items:center; z-index:9999;
    font-family:'IBM Plex Mono',monospace; }}
.pfl {{ font-size:0.66rem; color:{AMBER}; }} .pfr {{ font-size:0.57rem; color:#2e2e2e; }}
</style>
""", unsafe_allow_html=True)

PLOT = dict(
    paper_bgcolor=BG, plot_bgcolor=BG,
    font=dict(color="#777", family="IBM Plex Mono, monospace", size=10),
    margin=dict(l=40, r=10, t=28, b=35),
    xaxis=dict(gridcolor="#1e1e1e", linecolor="#222", zerolinecolor="#222"),
    yaxis=dict(gridcolor="#1e1e1e", linecolor="#222", zerolinecolor="#222"),
)

# -----------------------------------------------------------------------
# header
# -----------------------------------------------------------------------
st.markdown(
    '<p style="font-family:IBM Plex Mono,monospace;font-size:1rem;font-weight:600;color:#eee;margin:0.2rem 0 0.05rem">Parallax — Parallel VHDL Simulator</p>'
    f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.6rem;color:#333;margin:0 0 0.6rem">COD7001 / IIT Delhi / Chirag Kathpalia / 2025MCS2098 &nbsp;|&nbsp; detected {CPU_COUNT} logical CPUs</p>',
    unsafe_allow_html=True
)

# -----------------------------------------------------------------------
# control bar ... help= param gives hover tooltip on ?
# -----------------------------------------------------------------------
k1, k2, k3, k4, k5 = st.columns([3.2, 1.1, 0.9, 1, 1])

with k1:
    circuit_key = st.selectbox("Circuit", list(CIRCUITS.keys()),
        format_func=lambda k: f"{CIRCUITS[k]['name']}  ({CIRCUITS[k]['procs']} procs)",
        help="select the VHDL circuit to simulate. processes = number of concurrent VHDL processes in the design.")

with k2:
    stress = st.number_input("Stress (flops/gate)", min_value=0, max_value=2000000,
        value=0, step=1000,
        help="adds N floating-point multiplies per gate execution before running logic.\n\n"
             "models gate propagation delay ... real CMOS gates aren't instant.\n\n"
             "0 = ideal instant gates.\n"
             "10000 = ~83ns per gate (slow FPGA LUT).\n"
             "50000 = ~356ns per gate (complex logic cell).\n"
             "100000 = ~690ns per gate (slow TTL logic).\n\n"
             "use stress > 10000 on wide_and128 or hamming32 to see real parallel speedup.")

with k3:
    # default to min(8, cpu_count) for parallel
    default_idx = THREAD_OPTS.index(DEFAULT_TH) if DEFAULT_TH in THREAD_OPTS else len(THREAD_OPTS)-1
    threads = st.select_slider("Par threads", THREAD_OPTS,
        value=THREAD_OPTS[default_idx],
        help=f"number of OpenMP threads for parallel mode.\nauto-detected {CPU_COUNT} logical CPUs on this machine.\nphysical cores = ~{CPU_COUNT//2} ... going above physical cores may not help.")

with k4:
    st.write("")
    run_btn = st.button("Run  seq + par",
        help="runs BOTH sequential and parallel simulation and shows results side by side.")

with k5:
    st.write("")
    sweep_btn = st.button("Speedup sweep",
        help=f"runs seq + par at 1/2/4/8... threads up to {CPU_COUNT} and plots speedup curve.")

c = CIRCUITS[circuit_key]
st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#3d3d3d;margin:0 0 0.4rem">{c["desc"]}</p>', unsafe_allow_html=True)
st.divider()

# -----------------------------------------------------------------------
# run buttons
# -----------------------------------------------------------------------
if run_btn:
    with st.spinner("running sequential ..."):
        ss, sr = run_sim(circuit_key, "--seq", 1,       stress)
    with st.spinner(f"running parallel ({threads} threads) ..."):
        ps, pr = run_sim(circuit_key, "--par", threads,  stress)
    st.session_state.update({"ss":ss,"ps":ps,"sweep":None,"ck":circuit_key,"th":threads})

if sweep_btn:
    sweep = {}
    with st.spinner("running sequential baseline ..."):
        s0, _ = run_sim(circuit_key, "--seq", 1, stress)
        if s0 and "time" in s0: sweep["seq"] = s0
    for t in THREAD_OPTS[1:]:   # skip 1 ... already have seq baseline
        with st.spinner(f"running parallel {t} threads ..."):
            sp, _ = run_sim(circuit_key, "--par", t, stress)
            if sp and "time" in sp: sweep[f"par_{t}"] = sp
    # also run par/1 for overhead reference
    sp1, _ = run_sim(circuit_key, "--par", 1, stress)
    if sp1 and "time" in sp1: sweep["par_1"] = sp1
    st.session_state.update({"sweep":sweep,"ck":circuit_key,"th":threads,
                              "ss":sweep.get("seq",{}),"ps":sweep.get(f"par_{threads}",{})})

# -----------------------------------------------------------------------
# display results
# -----------------------------------------------------------------------
if "ss" in st.session_state and st.session_state["ss"]:
    ss    = st.session_state["ss"]
    ps    = st.session_state["ps"]
    sweep = st.session_state.get("sweep")
    th    = st.session_state.get("th", threads)
    ck    = st.session_state.get("ck", circuit_key)
    c     = CIRCUITS[ck]
    seq_t = ss.get("time", 0)
    par_t = ps.get("time", 0) if ps else 0

    # ---- derived metrics ----
    speedup      = round(seq_t / par_t, 3) if par_t > 0 else 0
    ideal_t      = seq_t / th if th > 0 else seq_t
    overhead_t   = max(0.0, par_t - ideal_t)
    useful_t     = max(0.0, par_t - overhead_t)
    overhead_pct = round(overhead_t / par_t * 100, 1) if par_t > 0 else 0
    useful_spdup = round(seq_t / useful_t, 2) if useful_t > 0 else 0
    efficiency   = round(speedup / th * 100, 1) if th > 0 else 0

    # ---- 4 charts ----
    st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.14em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.3rem">Analysis</p>', unsafe_allow_html=True)
    ca, cb, cc, cd = st.columns(4)

    # chart A ... wall time comparison
    with ca:
        fig = go.Figure()
        fig.add_trace(go.Bar(x=["seq"], y=[seq_t], marker_color=DIM,
            text=[f"{seq_t:.4f}s"], textposition="outside",
            textfont=dict(family="IBM Plex Mono", size=9, color="#777")))
        if par_t:
            fig.add_trace(go.Bar(x=[f"par ({th}T)"], y=[par_t], marker_color=AMBER,
                text=[f"{par_t:.4f}s"], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=9, color="#000")))
        fig.update_layout(**PLOT, title="wall time", showlegend=False,
                          yaxis_title="s", height=210)
        st.plotly_chart(fig, use_container_width=True)

    # chart B ... time breakdown: useful work vs sync overhead vs seq
    with cb:
        fig = go.Figure()
        fig.add_trace(go.Bar(x=["par"], y=[useful_t],   name="useful work",   marker_color=GREEN))
        fig.add_trace(go.Bar(x=["par"], y=[overhead_t], name="sync overhead", marker_color=RED))
        fig.add_trace(go.Bar(x=["seq"], y=[seq_t],      name="sequential",    marker_color=DIM))
        fig.update_layout(**PLOT, title="time breakdown", barmode="stack",
                          yaxis_title="s", height=210,
                          legend=dict(font=dict(family="IBM Plex Mono", size=8),
                                      orientation="h", y=-0.35, x=0))
        st.plotly_chart(fig, use_container_width=True)

    # chart C ... speedup comparison: actual vs useful-work speedup
    with cc:
        fig = go.Figure()
        if speedup > 0:
            fig.add_trace(go.Bar(
                x=["actual speedup", "useful work speedup"],
                y=[speedup, useful_spdup],
                marker_color=[AMBER if speedup >= 1 else RED, GREEN if useful_spdup >= 1 else RED],
                text=[f"{speedup:.2f}x", f"{useful_spdup:.2f}x"],
                textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#eee"),
            ))
            fig.add_hline(y=1.0, line_dash="dot", line_color="#444",
                          annotation_text="seq=1x",
                          annotation_font=dict(size=8, color="#555", family="IBM Plex Mono"))
        fig.update_layout(**PLOT, title="speedup", showlegend=False,
                          yaxis_title="speedup", height=210)
        st.plotly_chart(fig, use_container_width=True)

    # chart D ... sweep speedup vs threads OR process activity
    with cd:
        if sweep and "seq" in sweep:
            st_time = sweep["seq"]["time"]
            xs, ys, cols = [], [], []
            for t in THREAD_OPTS[1:]:
                key = f"par_{t}"
                if key in sweep:
                    sp = round(st_time / sweep[key]["time"], 2)
                    xs.append(str(t)); ys.append(sp)
                    cols.append(AMBER if sp >= 1 else RED)
            fig = go.Figure(go.Bar(x=xs, y=ys, marker_color=cols,
                text=[f"{y:.2f}x" for y in ys], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#eee")))
            fig.add_hline(y=1.0, line_dash="dot", line_color="#444",
                          annotation_text="seq baseline",
                          annotation_font=dict(size=8, color="#555", family="IBM Plex Mono"))
            fig.update_layout(**PLOT, title="speedup vs threads",
                              xaxis_title="threads", yaxis_title="speedup",
                              showlegend=False, height=210)
        else:
            fig = go.Figure()
            fig.add_trace(go.Bar(name="seq", x=["firings","deltas"],
                y=[ss.get("firings",0), ss.get("deltas",0)], marker_color=DIM))
            fig.add_trace(go.Bar(name="par", x=["firings","deltas"],
                y=[ps.get("firings",0) if ps else 0, ps.get("deltas",0) if ps else 0],
                marker_color=AMBER))
            fig.update_layout(**PLOT, title="activity (run sweep for speedup curve)",
                barmode="group", height=210,
                legend=dict(font=dict(family="IBM Plex Mono", size=8), orientation="h", y=-0.35))
        st.plotly_chart(fig, use_container_width=True)

    # ---- stat panels ----
    st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.14em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.3rem">Simulation Stats</p>', unsafe_allow_html=True)
    pl, pr_col = st.columns(2)

    # CHIRAG 22-04-26 :: root cause of left panel html bug
    # st.markdown with html in st.columns strips classes inconsistently
    # fix ... use ONLY native streamlit components inside panels
    # border/accent done via st.container context and a markdown line at top

    with pl:
        st.markdown('<div style="border-top:3px solid #3a3a3a;background:#181818;border:1px solid #242424;border-radius:6px;padding:0.1rem 0 0 0"><div style="border-top:3px solid #3a3a3a;border-radius:5px 5px 0 0"></div></div>', unsafe_allow_html=True)
        with st.container(border=True):
            st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.12em;text-transform:uppercase;color:#555;margin-bottom:0.3rem">Sequential</p>', unsafe_allow_html=True)
            st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:2rem;font-weight:600;color:#fff;margin:0;line-height:1">{seq_t:.4f}<span style="font-size:0.85rem;color:#555;margin-left:0.2rem">s</span></p>', unsafe_allow_html=True)

            exp = c.get("hash"); got = ss.get("hash", 0)
            if exp and got == exp:
                st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#4ade80;margin:0.4rem 0 0.6rem">hash {got}  verified</p>', unsafe_allow_html=True)
            else:
                st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#f87171;margin:0.4rem 0 0.6rem">hash mismatch  got {got}</p>', unsafe_allow_html=True)

            m1, m2 = st.columns(2)
            m1.metric("process firings", ss.get("firings","--"))
            m2.metric("delta cycles",    ss.get("deltas","--"))
            m3, m4 = st.columns(2)
            m3.metric("events",          ss.get("events","--"))
            m4.metric("max delta depth", ss.get("maxd","--"))
            m5, m6 = st.columns(2)
            m5.metric("par batches",     ss.get("colors","--"))
            m6.metric("dependencies",    ss.get("edges","--"))

    with pr_col:
        with st.container(border=True):
            st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.12em;text-transform:uppercase;color:#555;margin-bottom:0.3rem">Parallel / {th} threads</p>', unsafe_allow_html=True)

            if ps and "time" in ps:
                st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:2rem;font-weight:600;color:#fff;margin:0;line-height:1">{par_t:.4f}<span style="font-size:0.85rem;color:#555;margin-left:0.2rem">s</span></p>', unsafe_allow_html=True)

                badge_col = "#1a2e0d" if speedup >= 1 else "#2d0a0a"
                border_col= "#365314" if speedup >= 1 else "#7f1d1d"
                text_col  = "#86efac" if speedup >= 1 else "#fca5a5"
                st.markdown(f'<span style="display:inline-block;background:{badge_col};border:1px solid {border_col};color:{text_col};font-family:IBM Plex Mono,monospace;font-size:0.82rem;font-weight:600;padding:0.15rem 0.6rem;border-radius:3px;margin-bottom:0.4rem">{speedup:.2f}x speedup</span>', unsafe_allow_html=True)

                exp = c.get("hash"); got = ps.get("hash", 0)
                if exp and got == exp:
                    st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#4ade80;margin:0.1rem 0 0.6rem">hash {got}  verified</p>', unsafe_allow_html=True)
                else:
                    st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#f87171;margin:0.1rem 0 0.6rem">hash mismatch  got {got}</p>', unsafe_allow_html=True)

                n1, n2 = st.columns(2)
                n1.metric("process firings", ps.get("firings","--"))
                n2.metric("delta cycles",    ps.get("deltas","--"))
                n3, n4 = st.columns(2)
                n3.metric("events",          ps.get("events","--"))
                n4.metric("max delta depth", ps.get("maxd","--"))
                n5, n6 = st.columns(2)
                n5.metric("par batches",     ps.get("colors","--"))
                n6.metric("dependencies",    ps.get("edges","--"))

                st.divider()
                st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.1em;text-transform:uppercase;color:#444;margin-bottom:0.4rem">Parallelism Breakdown</p>', unsafe_allow_html=True)
                p1, p2 = st.columns(2)
                p1.metric("sync overhead",   f"{overhead_t:.4f}s", delta=f"{overhead_pct}% of par time", delta_color="inverse")
                p2.metric("useful work",     f"{useful_t:.4f}s",   delta=f"{useful_spdup:.2f}x vs seq")
                p3, p4 = st.columns(2)
                p3.metric("parallel efficiency", f"{efficiency}%",  help="speedup / threads * 100. 100% = perfect linear scaling.")
                p4.metric("useful speedup",  f"{useful_spdup:.2f}x", help="seq_time / useful_par_time. what speedup would look like if sync overhead were zero.")
            else:
                st.markdown('<p style="color:#444;font-family:IBM Plex Mono,monospace;font-size:0.8rem">parallel run failed or timed out</p>', unsafe_allow_html=True)

    # ---- dependency graph + gtkwave ----
    st.markdown("")
    dg1, dg2, _ = st.columns([1.3, 1.3, 6])
    with dg1:
        show_graph = st.button("Show dependency graph")
    with dg2:
        if st.button("Open in GTKWave",
                     help="opens the VCD waveform file in GTKWave. GTKWave must be installed and in PATH."):
            vcd = f"output-{ck}.vcd"
            if not os.path.exists(vcd):
                st.warning(f"{vcd} not found. run a simulation first.")
            else:
                try:
                    subprocess.Popen(["gtkwave", vcd],
                                     stdout=subprocess.DEVNULL,
                                     stderr=subprocess.DEVNULL)
                    st.success(f"launched GTKWave with {vcd}")
                except FileNotFoundError:
                    st.warning("GTKWave not found in PATH. on WSL: sudo apt install gtkwave")
                except Exception as e:
                    st.warning(f"could not open GTKWave: {e}")

    if show_graph:
        if os.path.exists("dependency.dot"):
            with open("dependency.dot") as f:
                dot = f.read()
            st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.6rem;letter-spacing:0.12em;text-transform:uppercase;color:#3a3a3a;margin-top:0.5rem">Dependency Graph — nodes=processes, edges=shared signals, color=parallel batch</p>', unsafe_allow_html=True)
            st.graphviz_chart(dot, use_container_width=False)
        else:
            st.info("no dependency.dot found. run a simulation first.")

else:
    # ---- default state ----
    import pandas as pd
    st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.14em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.4rem">Known Benchmarks / stress=100000 / 8 threads / WSL2</p>', unsafe_allow_html=True)
    dl, dr = st.columns([2, 1])
    with dl:
        df = pd.DataFrame([
            {"circuit":"thirty2_channel","processes":32, "par batches":1, "speedup/8T":"5.90x","note":"all independent, fewest delta cycles"},
            {"circuit":"hamming32",      "processes":32, "par batches":1, "speedup/8T":"4.71x","note":"32 fully independent parity processes"},
            {"circuit":"wide_and128",    "processes":128,"par batches":1, "speedup/8T":"3.93x","note":"128 independent AND gates"},
            {"circuit":"cla32",          "processes":97, "par batches":33,"speedup/8T":"1.75x","note":"31-stage carry chain limits speedup"},
            {"circuit":"pipeline8",      "processes":8,  "par batches":8, "speedup/8T":"< 1x", "note":"fully sequential, no parallelism"},
        ])
        st.dataframe(df, use_container_width=True, hide_index=True)
        st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#333;margin-top:0.5rem">1 parallel batch = all independent = scales with threads. many batches = Amdahl limit applies.</p>', unsafe_allow_html=True)
    with dr:
        if os.path.exists("dependency.dot"):
            st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.12em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.3rem">Last Dependency Graph</p>', unsafe_allow_html=True)
            with open("dependency.dot") as f:
                st.graphviz_chart(f.read(), use_container_width=True)

st.markdown('<div class="pf"><span class="pfl">sequential first. parallel second. correctness always.</span><span class="pfr">Chirag Kathpalia / 2025MCS2098 / COD7001 IIT Delhi / built with AI assistance (Claude, Anthropic)</span></div>', unsafe_allow_html=True)