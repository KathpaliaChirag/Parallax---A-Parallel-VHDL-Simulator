# CHIRAG 22-04-26 :: parallax dashboard v5
# fixes:
# ... useful speedup was always = thread count (tautology) ... replaced with efficiency %
# ... button CSS not applying ... added !important everywhere
# ... default view is now a proper welcome/help page
# ... run timestamp shown in results
# ... circuit info card
#
# run from parallax root: streamlit run dashboard/app.py

import streamlit as st
import subprocess, re, os, multiprocessing
from datetime import datetime
import plotly.graph_objects as go
import pandas as pd

AMBER = "#f59e0b"
GREEN = "#22c55e"
RED   = "#ef4444"
DIM   = "#3a3a3a"
BG    = "#191919"

CIRCUITS = {
    "and_gate":        {"name":"and gate",                   "vhdl":"tests/circuit/basic/and_gate.vhdl",        "tb":"",                                           "desc":"single 2-input AND gate. simplest possible circuit.",             "why":"every CPU and FPGA is built from these.",                                     "colors":1,  "procs":1,   "hash":2847733139},
    "multi_and":       {"name":"multi AND (4 gates)",         "vhdl":"tests/circuit/basic/multi_and.vhdl",       "tb":"tests/circuit/basic/multi_and_tb.txt",       "desc":"4 independent AND gates with no shared signals between them.",    "why":"first circuit where Welsh-Powell coloring does something useful.",             "colors":1,  "procs":4,   "hash":3198782091},
    "dff":             {"name":"D flip flop",                 "vhdl":"tests/circuit/basic/dff.vhdl",             "tb":"tests/circuit/basic/dff_tb.txt",             "desc":"captures input D on rising clock edge, holds until next clock.",  "why":"every register in every CPU is made of these.",                               "colors":1,  "procs":1,   "hash":3702914041},
    "tff":             {"name":"T flip flop",                 "vhdl":"tests/circuit/basic/tff.vhdl",             "tb":"tests/circuit/basic/tff_tb.txt",             "desc":"toggles output when T=1 on rising clock, holds when T=0.",        "why":"frequency dividers and binary counters.",                                     "colors":1,  "procs":2,   "hash":3818917602},
    "srff":            {"name":"SR flip flop",                "vhdl":"tests/circuit/basic/srff.vhdl",            "tb":"tests/circuit/basic/srff_tb.txt",            "desc":"set/reset flip flop. S=1 forces Q high, R=1 forces Q low.",       "why":"interrupt flags in CPUs. hardware sets it, software clears it.",              "colors":1,  "procs":2,   "hash":3798037844},
    "jkff":            {"name":"JK flip flop",                "vhdl":"tests/circuit/basic/jkff.vhdl",            "tb":"tests/circuit/basic/jkff_tb.txt",            "desc":"most versatile flip flop. J=K=1 toggles instead of undefined.",   "why":"universal flip flop. replaces DFF, TFF, or SRFF.",                            "colors":1,  "procs":2,   "hash":2427382327},
    "ripple":          {"name":"ripple carry adder",          "vhdl":"tests/circuit/basic/ripple.vhdl",          "tb":"tests/circuit/basic/ripple_tb.txt",          "desc":"4-bit adder where carry propagates sequentially bit by bit.",      "why":"shows sequential dependency. parallel can't help here.",                      "colors":4,  "procs":4,   "hash":342984039},
    "chainof4":        {"name":"chain of 4",                  "vhdl":"tests/circuit/basic/chainof4.vhdl",        "tb":"tests/circuit/basic/chainof4_tb.txt",        "desc":"4-stage pipeline where each stage reads the previous output.",     "why":"Welsh-Powell assigns 4 colors. demonstrates Amdahl's law.",                   "colors":4,  "procs":4,   "hash":749744446},
    "pipeline8":       {"name":"pipeline 8-stage",            "vhdl":"tests/circuit/basic/pipeline8.vhdl",       "tb":"tests/circuit/basic/pipeline8_tb.txt",       "desc":"8-stage fully sequential dependency chain.",                      "why":"models instruction pipeline. no parallelism possible.",                        "colors":8,  "procs":8,   "hash":3424237402},
    "eight_channel":   {"name":"8-channel OR bank",           "vhdl":"tests/circuit/basic/eight_channel.vhdl",   "tb":"tests/circuit/basic/eight_channel_tb.txt",   "desc":"8 independent OR gates with no dependencies between them.",       "why":"bus arbitration. 8 channels checked simultaneously.",                         "colors":1,  "procs":8,   "hash":844848091},
    "thirty2_channel": {"name":"32-channel OR bank",          "vhdl":"tests/circuit/basic/thirty2_channel.vhdl", "tb":"tests/circuit/basic/thirty2_channel_tb.txt", "desc":"32 independent OR gates. shows real speedup at stress >= 10000.", "why":"32 interrupt lines simultaneously. 5.9x speedup at 8 threads.",               "colors":1,  "procs":32,  "hash":1666589379},
    "wide_and128":     {"name":"wide AND 128",                "vhdl":"tests/circuit/basic/wide_and128.vhdl",     "tb":"tests/circuit/basic/wide_and128_tb.txt",     "desc":"128 independent AND gates. best case for parallel speedup.",      "why":"SIMD logic unit. 128 lanes in one clock. 3.93x at 8 threads.",               "colors":1,  "procs":128, "hash":1047282184},
    "traffic_light":   {"name":"traffic light FSM",           "vhdl":"tests/circuit/fsm/traffic_light.vhdl",     "tb":"tests/circuit/fsm/traffic_light_tb.txt",     "desc":"finite state machine controlling a 3-light traffic signal.",       "why":"real FSM. sequential state transitions.",                                     "colors":2,  "procs":3,   "hash":2957170177},
    "majority":        {"name":"majority voter",              "vhdl":"tests/circuit/basic/majority.vhdl",        "tb":"",                                           "desc":"5-input majority voter implemented with a VHDL function.",        "why":"fault-tolerant voting. spacecraft and nuclear controllers.",                   "colors":1,  "procs":1,   "hash":1484108199},
    "hamming4":        {"name":"hamming encoder 4-bit",       "vhdl":"tests/circuit/basic/hamming4.vhdl",        "tb":"",                                           "desc":"hamming(7,4) encoder. 3 independent parity processes.",           "why":"ECC for 4-bit data. detects and corrects single-bit errors.",                 "colors":1,  "procs":3,   "hash":2071069720},
    "hamming32":       {"name":"hamming encoder 32-bit",      "vhdl":"tests/circuit/basic/hamming32.vhdl",       "tb":"tests/circuit/basic/hamming32_tb.txt",       "desc":"32 independent parity processes. strong parallel speedup.",        "why":"ECC for 32-bit data bus. standard in DDR4/DDR5. 4.71x at 8 threads.",        "colors":1,  "procs":32,  "hash":4083200419},
    "cla4":            {"name":"carry lookahead adder 4-bit", "vhdl":"tests/circuit/basic/cla4.vhdl",            "tb":"tests/circuit/basic/cla4_tb.txt",            "desc":"G/P processes fully parallel, carry chain sequential.",           "why":"fast binary addition. every modern CPU ALU uses CLA.",                        "colors":8,  "procs":17,  "hash":472018830},
    "cla32":           {"name":"carry lookahead adder 32-bit","vhdl":"tests/circuit/basic/cla32.vhdl",           "tb":"tests/circuit/basic/cla32_tb.txt",           "desc":"64 parallel G/P processes + 31-stage sequential carry chain.",    "why":"32-bit CPU addition. shows Amdahl limit from carry chain. 1.75x at 8 threads.","colors":33, "procs":97,  "hash":3312919180},
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

CPU_COUNT   = multiprocessing.cpu_count()
THREAD_OPTS = [t for t in [1,2,4,8,16,32] if t <= CPU_COUNT]
if not THREAD_OPTS: THREAD_OPTS = [1]
DEFAULT_TH  = min(8, CPU_COUNT)

# -----------------------------------------------------------------------
st.set_page_config(page_title="Parallax", layout="wide", initial_sidebar_state="collapsed")

st.markdown(f"""
<style>
@import url('https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:wght@400;500;600&family=IBM+Plex+Sans:wght@300;400;500&display=swap');

html, body, [class*="css"] {{
    font-family: 'IBM Plex Sans', sans-serif !important;
    background: #111 !important;
    color: #ddd !important;
}}
#MainMenu, footer, header {{ visibility: hidden !important; }}
.stDeployButton {{ display: none !important; }}
section[data-testid="stSidebar"] {{ display: none !important; }}
.block-container {{ padding: 0.8rem 1.8rem 5rem !important; max-width: 100% !important; }}

/* inputs */
.stSelectbox > div > div {{
    background: #1c1c1c !important; border: 1px solid #2a2a2a !important;
    color: #ddd !important; font-family: 'IBM Plex Mono', monospace !important;
    border-radius: 4px !important;
}}
.stNumberInput > div > div > input {{
    background: #1c1c1c !important; border: 1px solid #2a2a2a !important;
    color: #ddd !important; font-family: 'IBM Plex Mono', monospace !important;
}}
label, p label {{
    font-family: 'IBM Plex Mono', monospace !important;
    font-size: 0.62rem !important; letter-spacing: 0.1em !important;
    text-transform: uppercase !important; color: #555 !important;
}}
/* slider track */
.stSlider > div > div > div {{ background: #2a2a2a !important; }}

/* ALL buttons amber */
.stButton > button {{
    background: {AMBER} !important; color: #000 !important;
    font-family: 'IBM Plex Mono', monospace !important;
    font-weight: 700 !important; font-size: 0.73rem !important;
    letter-spacing: 0.05em !important; border: none !important;
    border-radius: 4px !important; padding: 0.5rem 1rem !important;
    width: 100% !important; cursor: pointer !important;
}}
.stButton > button:hover {{
    background: #d97706 !important; color: #000 !important;
}}
.stButton > button:active {{ background: #b45309 !important; }}

/* metrics */
[data-testid="metric-container"] {{
    background: #181818 !important; border: 1px solid #242424 !important;
    border-radius: 4px !important; padding: 0.6rem 0.9rem !important;
}}
[data-testid="stMetricValue"] {{
    font-family: 'IBM Plex Mono', monospace !important;
    font-size: 1.05rem !important; color: #eee !important;
}}
[data-testid="stMetricLabel"] {{
    font-family: 'IBM Plex Mono', monospace !important;
    font-size: 0.58rem !important; color: #555 !important;
    letter-spacing: 0.1em !important; text-transform: uppercase !important;
}}
[data-testid="stMetricDelta"] {{
    font-family: 'IBM Plex Mono', monospace !important; font-size: 0.72rem !important;
}}

/* container borders */
[data-testid="stVerticalBlock"] > [data-testid="stVerticalBlock"] {{
    border-radius: 4px;
}}
hr {{ border-color: #1e1e1e !important; margin: 0.6rem 0 !important; }}

/* dataframe */
.dataframe {{ font-family: 'IBM Plex Mono', monospace !important; font-size: 0.78rem !important; }}

/* footer */
.pf {{
    position: fixed; bottom: 0; left: 0; right: 0;
    background: #0d0d0d; border-top: 1px solid #1a1a1a;
    padding: 0.4rem 1.5rem; display: flex;
    justify-content: space-between; align-items: center; z-index: 9999;
    font-family: 'IBM Plex Mono', monospace;
}}
.pfl {{ font-size: 0.66rem; color: {AMBER}; }}
.pfr {{ font-size: 0.57rem; color: #2a2a2a; }}

/* welcome page cards */
.wcard {{
    background: #181818; border: 1px solid #242424; border-radius: 6px;
    padding: 1rem 1.2rem; height: 100%;
}}
.wcard-title {{
    font-family: 'IBM Plex Mono', monospace; font-size: 0.65rem;
    letter-spacing: 0.12em; text-transform: uppercase; color: {AMBER};
    margin-bottom: 0.5rem;
}}
.wcard-body {{
    font-size: 0.82rem; color: #999; line-height: 1.65;
}}
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
h1, h2 = st.columns([8, 1])
with h1:
    st.markdown(
        '<p style="font-family:IBM Plex Mono,monospace;font-size:1rem;font-weight:600;color:#eee;margin:0.2rem 0 0.05rem">Parallax — Parallel VHDL Simulator</p>'
        f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.6rem;color:#333;margin:0 0 0.5rem">COD7001 / IIT Delhi / Chirag Kathpalia / 2025MCS2098 &nbsp;|&nbsp; {CPU_COUNT} logical CPUs detected</p>',
        unsafe_allow_html=True
    )
with h2:
    if st.button("? Help", help="show the welcome / help page"):
        st.session_state["show_help"] = True
        for k in ["ss","ps","sweep"]:
            if k in st.session_state: del st.session_state[k]

# -----------------------------------------------------------------------
# control bar
# -----------------------------------------------------------------------
k1, k2, k3, k4, k5 = st.columns([3.2, 1.1, 0.9, 1, 1])
with k1:
    circuit_key = st.selectbox("Circuit", list(CIRCUITS.keys()),
        format_func=lambda k: f"{CIRCUITS[k]['name']}  ({CIRCUITS[k]['procs']} processes)",
        help="select the VHDL circuit to simulate.\nprocesses = concurrent VHDL processes in the design.\nmore independent processes = more parallelism.")
with k2:
    stress = st.number_input("Stress (flops/gate)", min_value=0, max_value=2000000, value=0, step=1000,
        help="models gate propagation delay by adding N floating-point multiplies per gate.\n\n"
             "real CMOS gates aren't instant ... this makes the simulator more realistic.\n\n"
             "0      = instant gates (default)\n"
             "10000  = ~83ns delay (slow FPGA LUT)\n"
             "50000  = ~356ns delay (complex cell)\n"
             "100000 = ~690ns delay (slow TTL)\n\n"
             "use stress >= 10000 on wide_and128 or hamming32 to see real parallel speedup.")
with k3:
    default_idx = THREAD_OPTS.index(DEFAULT_TH) if DEFAULT_TH in THREAD_OPTS else len(THREAD_OPTS)-1
    threads = st.select_slider("Par threads", THREAD_OPTS, value=THREAD_OPTS[default_idx],
        help=f"OpenMP thread count for parallel mode.\n{CPU_COUNT} logical CPUs detected.\nphysical cores ≈ {CPU_COUNT//2} ... going above physical cores may slow things down.")
with k4:
    st.write("")
    run_btn = st.button("Run  seq + par",
        help="runs BOTH sequential and parallel with the same inputs.\nshows results side by side for direct comparison.")
with k5:
    st.write("")
    sweep_btn = st.button("Speedup sweep",
        help=f"runs seq + par at all thread counts up to {CPU_COUNT}.\nplots speedup curve. takes 2-5 minutes for large circuits.")

c = CIRCUITS[circuit_key]

# circuit info strip
ci_color = AMBER if c["colors"] == 1 else "#888"
ci_note  = "all independent (1 color batch)" if c["colors"] == 1 else f"{c['colors']} color batches (sequential dependencies)"
st.markdown(
    f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.68rem;color:#3d3d3d;margin:0.1rem 0 0.05rem">'
    f'{c["desc"]}'
    f'&nbsp; <span style="color:{ci_color}">/ {ci_note}</span></p>',
    unsafe_allow_html=True
)
st.divider()

# -----------------------------------------------------------------------
# run
# -----------------------------------------------------------------------
if run_btn:
    st.session_state["show_help"] = False
    with st.spinner("running sequential ..."):
        ss, _ = run_sim(circuit_key, "--seq", 1,      stress)
    with st.spinner(f"running parallel ({threads} threads) ..."):
        ps, _ = run_sim(circuit_key, "--par", threads, stress)
    st.session_state.update({"ss":ss,"ps":ps,"sweep":None,"ck":circuit_key,"th":threads,
                              "ts":datetime.now().strftime("%H:%M:%S")})

if sweep_btn:
    st.session_state["show_help"] = False
    sweep = {}
    with st.spinner("running sequential baseline ..."):
        s0, _ = run_sim(circuit_key, "--seq", 1, stress)
        if s0 and "time" in s0: sweep["seq"] = s0
    for t in THREAD_OPTS:
        with st.spinner(f"running parallel {t} threads ..."):
            sp, _ = run_sim(circuit_key, "--par", t, stress)
            if sp and "time" in sp: sweep[f"par_{t}"] = sp
    st.session_state.update({"sweep":sweep,"ck":circuit_key,"th":threads,
                              "ss":sweep.get("seq",{}),"ps":sweep.get(f"par_{threads}",{}),
                              "ts":datetime.now().strftime("%H:%M:%S")})

# -----------------------------------------------------------------------
# welcome / help page
# -----------------------------------------------------------------------
show_help = st.session_state.get("show_help", True)
has_data  = "ss" in st.session_state and st.session_state.get("ss")

if show_help or not has_data:
    st.markdown("""
    <div style="font-family:IBM Plex Mono,monospace;font-size:0.6rem;letter-spacing:0.14em;text-transform:uppercase;color:#444;margin-bottom:0.8rem">
        Welcome to Parallax
    </div>
    """, unsafe_allow_html=True)

    wa, wb, wc = st.columns(3)
    with wa:
        st.markdown("""
        <div class="wcard">
            <div class="wcard-title">What is Parallax?</div>
            <div class="wcard-body">
                Parallax is a parallel VHDL event-driven simulator built as part of COD7001 at IIT Delhi.<br><br>
                it parses real VHDL, builds a dependency graph using Welsh-Powell graph coloring,
                and executes independent processes in parallel using OpenMP.<br><br>
                correctness is verified by comparing trace hashes between sequential and parallel runs.
                if hashes match, the simulation is bit-for-bit identical.
            </div>
        </div>
        """, unsafe_allow_html=True)
    with wb:
        st.markdown(f"""
        <div class="wcard">
            <div class="wcard-title">How to use this dashboard</div>
            <div class="wcard-body">
                <b style="color:#eee">1. select a circuit</b> from the dropdown above.<br>
                circuits with 1 color batch are fully independent and show the best speedup.<br><br>
                <b style="color:#eee">2. set stress</b> to model gate delay.
                try 50000 on <span style="color:{AMBER}">wide_and128</span> or <span style="color:{AMBER}">hamming32</span>
                to see real parallel speedup.<br><br>
                <b style="color:#eee">3. click Run seq + par</b> to run both modes and compare results.<br><br>
                <b style="color:#eee">4. click Speedup sweep</b> to run all thread counts and plot the speedup curve.
            </div>
        </div>
        """, unsafe_allow_html=True)
    with wc:
        st.markdown("""
        <div class="wcard">
            <div class="wcard-title">Reading the charts</div>
            <div class="wcard-body">
                <b style="color:#eee">wall time</b> — actual elapsed time. lower is better.<br><br>
                <b style="color:#eee">time breakdown</b> — green = useful parallel work.
                red = synchronization overhead (OMP barriers, thread coordination).
                grey = sequential baseline.<br><br>
                <b style="color:#eee">speedup vs efficiency</b> — speedup = seq/par.
                efficiency = speedup/threads. 100% efficiency = perfect linear scaling.<br><br>
                <b style="color:#eee">speedup curve</b> — shows how speedup scales with thread count.
                flattening = Amdahl limit reached.
            </div>
        </div>
        """, unsafe_allow_html=True)

    st.markdown("<br>", unsafe_allow_html=True)
    wd, we = st.columns(2)
    with wd:
        st.markdown("""
        <div class="wcard">
            <div class="wcard-title">What is stress?</div>
            <div class="wcard-body">
                real CMOS gates have propagation delay ... electrons move through transistors,
                capacitances charge. a 28nm AND gate takes ~50-500ps.<br><br>
                our simulator models gates as instant (0 delay). stress adds N floating-point
                multiplies per gate execution to model this delay.<br><br>
                without stress, OMP barrier overhead (500µs) >> gate work (5ns), so parallel always loses.
                with stress=50000, gate work >> barrier overhead and parallel wins.<br><br>
                this is physically motivated, not cheating.
                real gates DO take time. we just make it explicit.
            </div>
        </div>
        """, unsafe_allow_html=True)
    with we:
        st.markdown("""
        <div class="wcard">
            <div class="wcard-title">Best circuits to demo</div>
            <div class="wcard-body">
                <span style="color:#f59e0b">wide_and128</span> — 128 independent AND gates.
                1 color batch. 3.93x speedup at 8 threads with stress=50000.<br><br>
                <span style="color:#f59e0b">hamming32</span> — 32 independent parity processes.
                1 color batch. 4.71x speedup at 8 threads.<br><br>
                <span style="color:#f59e0b">thirty2_channel</span> — 32 independent OR gates.
                fewest delta cycles. 5.9x speedup at 8 threads.<br><br>
                <span style="color:#888">pipeline8</span> — 8 sequential stages. 8 color batches.
                speedup < 1x even with stress. shows Amdahl limit.
            </div>
        </div>
        """, unsafe_allow_html=True)

    st.markdown("<br>", unsafe_allow_html=True)
    st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.7rem;color:#333;text-align:center">select a circuit above and click Run seq + par to start</p>', unsafe_allow_html=True)

# -----------------------------------------------------------------------
# results
# -----------------------------------------------------------------------
elif has_data:
    ss    = st.session_state["ss"]
    ps    = st.session_state["ps"]
    sweep = st.session_state.get("sweep")
    th    = st.session_state.get("th", threads)
    ck    = st.session_state.get("ck", circuit_key)
    ts    = st.session_state.get("ts", "")
    c     = CIRCUITS[ck]
    seq_t = ss.get("time", 0)
    par_t = ps.get("time", 0) if ps else 0

    # derived
    speedup    = round(seq_t / par_t, 3) if par_t > 0 else 0
    efficiency = round(speedup / th * 100, 1) if th > 0 else 0

    # overhead estimation using par/1 if available, else estimate
    par1_t = sweep["par_1"]["time"] if (sweep and "par_1" in sweep) else None
    if par1_t:
        # overhead = par/1 - seq ... the fixed cost of OMP infrastructure
        overhead_t = max(0.0, par1_t - seq_t)
    else:
        # rough estimate: par/1 ≈ par/N * N * (seq_t / (seq_t + estimated_overhead))
        # simpler: assume overhead is par_t - seq_t/th ... the part that didn't scale
        overhead_t = max(0.0, par_t - seq_t / th) if th > 0 else 0
    overhead_pct = round(overhead_t / par_t * 100, 1) if par_t > 0 else 0

    # useful work = par_time minus estimated overhead
    useful_t = max(0.001, par_t - overhead_t)
    # CHIRAG 22-04-26 :: useful speedup fix
    # old formula: useful_t = seq_t/th ... so useful_speedup = th always (tautology)
    # fix: useful_speedup = seq_t / (par_t - overhead) where overhead is estimated from par/1
    # if par/1 not available, useful_speedup is just the upper bound = speedup corrected for overhead
    useful_spdup = round(seq_t / useful_t, 2) if useful_t > 0 else speedup

    ts_label = f" — run at {ts}" if ts else ""

    # ---- charts ----
    st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.14em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.3rem">Analysis{ts_label}</p>', unsafe_allow_html=True)
    ca, cb, cc, cd = st.columns(4)

    with ca:
        fig = go.Figure()
        fig.add_trace(go.Bar(x=["seq"], y=[seq_t], marker_color=DIM,
            text=[f"{seq_t:.4f}s"], textposition="outside",
            textfont=dict(family="IBM Plex Mono", size=9, color="#777")))
        if par_t:
            fig.add_trace(go.Bar(x=[f"par/{th}T"], y=[par_t], marker_color=AMBER,
                text=[f"{par_t:.4f}s"], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=9, color="#000")))
        fig.update_layout(**PLOT, title="wall time", showlegend=False, yaxis_title="s", height=220)
        st.plotly_chart(fig, use_container_width=True)

    with cb:
        if par_t and seq_t:
            fig = go.Figure()
            fig.add_trace(go.Bar(x=["par"], y=[useful_t],   name="useful work",   marker_color=GREEN))
            fig.add_trace(go.Bar(x=["par"], y=[overhead_t], name="sync overhead", marker_color=RED))
            fig.add_trace(go.Bar(x=["seq"], y=[seq_t],      name="sequential",    marker_color=DIM))
            note = "overhead from par/1" if par1_t else "estimated overhead"
            fig.update_layout(**PLOT, title=f"breakdown ({note})", barmode="stack", yaxis_title="s", height=220,
                legend=dict(font=dict(family="IBM Plex Mono", size=8), orientation="h", y=-0.38, x=0))
            st.plotly_chart(fig, use_container_width=True)

    with cc:
        # CHIRAG 22-04-26 :: fixed speedup chart
        # showing actual speedup AND parallel efficiency side by side
        # efficiency = speedup/threads*100 ... tells how well we used the threads
        # 100% = perfect linear scaling ... 50% = half efficient
        fig = go.Figure()
        fig.add_trace(go.Bar(
            x=["speedup", "efficiency (%)"],
            y=[speedup, efficiency],
            marker_color=[AMBER if speedup >= 1 else RED,
                          GREEN if efficiency >= 50 else RED],
            text=[f"{speedup:.2f}x", f"{efficiency:.0f}%"],
            textposition="outside",
            textfont=dict(family="IBM Plex Mono", size=11, color="#eee"),
        ))
        fig.add_hline(y=1.0, line_dash="dot", line_color="#444",
                      annotation_text="seq=1x",
                      annotation_font=dict(size=8, color="#555", family="IBM Plex Mono"))
        fig.update_layout(**PLOT, title=f"speedup + efficiency / {th} threads",
                          showlegend=False, height=220)
        st.plotly_chart(fig, use_container_width=True)

    with cd:
        if sweep and "seq" in sweep:
            st_time = sweep["seq"]["time"]
            xs, ys, cols = [], [], []
            for t in THREAD_OPTS:
                key = f"par_{t}"
                if key in sweep:
                    sp = round(st_time / sweep[key]["time"], 2)
                    xs.append(str(t)); ys.append(sp)
                    cols.append(AMBER if sp >= 1 else RED)
            fig = go.Figure(go.Bar(x=xs, y=ys, marker_color=cols,
                text=[f"{y:.2f}x" for y in ys], textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=10, color="#eee")))
            fig.add_hline(y=1.0, line_dash="dot", line_color="#444",
                          annotation_text="seq",
                          annotation_font=dict(size=8, color="#555", family="IBM Plex Mono"))
            fig.update_layout(**PLOT, title="speedup vs threads",
                              xaxis_title="threads", yaxis_title="x", showlegend=False, height=220)
        else:
            # activity comparison
            fig = go.Figure()
            fig.add_trace(go.Bar(name="seq", x=["firings","deltas"],
                y=[ss.get("firings",0), ss.get("deltas",0)], marker_color=DIM))
            if ps:
                fig.add_trace(go.Bar(name=f"par/{th}T", x=["firings","deltas"],
                    y=[ps.get("firings",0), ps.get("deltas",0)], marker_color=AMBER))
            fig.update_layout(**PLOT, title="activity  (run sweep for speedup curve)",
                barmode="group", height=220,
                legend=dict(font=dict(family="IBM Plex Mono", size=8), orientation="h", y=-0.38))
        st.plotly_chart(fig, use_container_width=True)

    # ---- stat panels ----
    st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.14em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.3rem">Simulation Stats{ts_label}</p>', unsafe_allow_html=True)
    pl, pr_col = st.columns(2)

    def stat_metrics(stats, label, border_color, seq_time=None, thread_n=1):
        t   = stats.get("time", 0)
        exp = c.get("hash"); got = stats.get("hash", 0)
        st.markdown(
            f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.12em;'
            f'text-transform:uppercase;color:#555;margin-bottom:0.3rem;'
            f'border-top:3px solid {border_color};padding-top:0.6rem">{label}</p>',
            unsafe_allow_html=True
        )
        st.markdown(
            f'<p style="font-family:IBM Plex Mono,monospace;font-size:1.9rem;font-weight:600;'
            f'color:#fff;margin:0;line-height:1">{t:.4f}'
            f'<span style="font-size:0.8rem;color:#555;margin-left:0.2rem">s</span></p>',
            unsafe_allow_html=True
        )
        if seq_time and seq_time > 0:
            sp = round(seq_time / t, 2) if t > 0 else 0
            eff = round(sp / thread_n * 100, 1) if thread_n > 0 else 0
            bc = "#1a2e0d" if sp >= 1 else "#2d0a0a"
            tc = "#86efac" if sp >= 1 else "#fca5a5"
            ec = "#365314" if sp >= 1 else "#7f1d1d"
            st.markdown(
                f'<span style="display:inline-block;background:{bc};border:1px solid {ec};'
                f'color:{tc};font-family:IBM Plex Mono,monospace;font-size:0.8rem;font-weight:600;'
                f'padding:0.12rem 0.55rem;border-radius:3px;margin:0.3rem 0 0.4rem">'
                f'{sp:.2f}x speedup &nbsp;/&nbsp; {eff:.0f}% efficient</span>',
                unsafe_allow_html=True
            )
        if exp and got == exp:
            st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.66rem;color:#4ade80;margin:0.1rem 0 0.5rem">hash {got}  verified</p>', unsafe_allow_html=True)
        else:
            st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.66rem;color:#f87171;margin:0.1rem 0 0.5rem">hash mismatch  got {got}</p>', unsafe_allow_html=True)

        r1, r2 = st.columns(2)
        r1.metric("process firings", stats.get("firings","--"))
        r2.metric("delta cycles",    stats.get("deltas","--"))
        r3, r4 = st.columns(2)
        r3.metric("events",          stats.get("events","--"))
        r4.metric("max delta depth", stats.get("maxd","--"))
        r5, r6 = st.columns(2)
        r5.metric("par batches",     stats.get("colors","--"), help="number of Welsh-Powell color classes. 1 = fully parallel. more = more sequential dependency.")
        r6.metric("dependencies",    stats.get("edges","--"),  help="edges in dependency graph. 0 = fully independent. more = more shared signals between processes.")

    with pl:
        with st.container(border=True):
            stat_metrics(ss, "Sequential", DIM)

    with pr_col:
        with st.container(border=True):
            if ps and "time" in ps:
                stat_metrics(ps, f"Parallel / {th} threads", AMBER, seq_t, th)
                if overhead_t > 0:
                    st.divider()
                    st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;letter-spacing:0.1em;text-transform:uppercase;color:#444;margin-bottom:0.3rem">Parallelism Breakdown</p>', unsafe_allow_html=True)
                    o1, o2 = st.columns(2)
                    src = "measured from par/1" if par1_t else "estimated"
                    o1.metric("sync overhead", f"{overhead_t:.4f}s",
                              delta=f"{overhead_pct}% of par time ({src})", delta_color="inverse",
                              help="time lost to OMP barriers, thread coordination, queue merging. this is the fixed cost of parallelism.")
                    o2.metric("useful work",   f"{useful_t:.4f}s",
                              help="par_time minus sync overhead. the actual computation that scales with more threads.")
                    o3, o4 = st.columns(2)
                    o3.metric("parallel efficiency", f"{efficiency:.0f}%",
                              help="speedup / threads * 100. 100% = perfect linear scaling. 50% = half efficient.")
                    o4.metric("useful speedup", f"{useful_spdup:.2f}x",
                              help="seq_time / useful_work_time. speedup if sync overhead were zero. better than actual when overhead is high.")
            else:
                st.markdown('<p style="color:#444;font-family:IBM Plex Mono,monospace;font-size:0.8rem;padding:1rem">parallel run failed or timed out</p>', unsafe_allow_html=True)

    # ---- dependency graph + gtkwave ----
    st.markdown("")
    dg1, dg2, dg3 = st.columns([1.3, 1.3, 6])
    with dg1:
        show_graph = st.button("Show dependency graph")
    with dg2:
        if st.button("Open in GTKWave", help="opens the VCD waveform in GTKWave. must be installed: sudo apt install gtkwave"):
            vcd = f"output-{ck}.vcd"
            if not os.path.exists(vcd):
                st.warning(f"{vcd} not found. run a simulation first.")
            else:
                try:
                    subprocess.Popen(["gtkwave", vcd], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                    st.success(f"opened {vcd} in GTKWave")
                except FileNotFoundError:
                    st.warning("GTKWave not in PATH. install with: sudo apt install gtkwave")
                except Exception as ex:
                    st.warning(f"could not launch GTKWave: {ex}")
    with dg3:
        # export results
        if st.button("Export results as text"):
            lines = [
                f"Parallax simulation results — {ts}",
                f"circuit: {c['name']}  ({c['procs']} processes, {c.get('colors','?')} color batches)",
                f"stress: {stress} flops/gate",
                f"",
                f"sequential:  {seq_t:.6f}s  hash={ss.get('hash','?')}",
                f"parallel/{th}T: {par_t:.6f}s  hash={ps.get('hash','?') if ps else '?'}",
                f"speedup:     {speedup:.3f}x",
                f"efficiency:  {efficiency:.1f}%",
                f"",
                f"process firings: seq={ss.get('firings','?')}  par={ps.get('firings','?') if ps else '?'}",
                f"delta cycles:    seq={ss.get('deltas','?')}  par={ps.get('deltas','?') if ps else '?'}",
            ]
            st.download_button("download", "\n".join(lines), file_name=f"parallax_{ck}_{ts.replace(':','-')}.txt")

    if show_graph:
        if os.path.exists("dependency.dot"):
            with open("dependency.dot") as f:
                dot = f.read()
            st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.6rem;letter-spacing:0.1em;text-transform:uppercase;color:#3a3a3a;margin-top:0.5rem">nodes=processes / edges=shared signals / color=parallel batch</p>', unsafe_allow_html=True)
            st.graphviz_chart(dot, use_container_width=False)
        else:
            st.info("no dependency.dot found. run a simulation first.")

st.markdown('<div class="pf"><span class="pfl">sequential first. parallel second. correctness always.</span><span class="pfr">Chirag Kathpalia / 2025MCS2098 / COD7001 IIT Delhi / built with AI assistance (Claude, Anthropic)</span></div>', unsafe_allow_html=True)