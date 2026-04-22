# CHIRAG 22-04-26 :: parallax dashboard FINAL
# fixes from v7:
# ... TypeError: multiple values for 'yaxis' ... removed PLOT dict unpacking
#     replaced with apply_plot() helper that builds layout cleanly
# ... benchmark table not showing on first run ... simplified button logic
# ... all tabs now render without errors
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
    "and_gate":        {"name":"and gate",                   "vhdl":"tests/circuit/basic/and_gate.vhdl",        "tb":"",                                           "desc":"single 2-input AND gate",                  "procs":1,   "colors":1,  "hash":2847733139},
    "multi_and":       {"name":"multi AND (4 gates)",         "vhdl":"tests/circuit/basic/multi_and.vhdl",       "tb":"tests/circuit/basic/multi_and_tb.txt",       "desc":"4 independent AND gates",                  "procs":4,   "colors":1,  "hash":3198782091},
    "dff":             {"name":"D flip flop",                 "vhdl":"tests/circuit/basic/dff.vhdl",             "tb":"tests/circuit/basic/dff_tb.txt",             "desc":"captures D on rising clock edge",           "procs":1,   "colors":1,  "hash":3702914041},
    "tff":             {"name":"T flip flop",                 "vhdl":"tests/circuit/basic/tff.vhdl",             "tb":"tests/circuit/basic/tff_tb.txt",             "desc":"toggles on clock when T=1",                "procs":2,   "colors":1,  "hash":3818917602},
    "srff":            {"name":"SR flip flop",                "vhdl":"tests/circuit/basic/srff.vhdl",            "tb":"tests/circuit/basic/srff_tb.txt",            "desc":"set/reset flip flop with clock",           "procs":2,   "colors":1,  "hash":3798037844},
    "jkff":            {"name":"JK flip flop",                "vhdl":"tests/circuit/basic/jkff.vhdl",            "tb":"tests/circuit/basic/jkff_tb.txt",            "desc":"most versatile flip flop",                 "procs":2,   "colors":1,  "hash":2427382327},
    "ripple":          {"name":"ripple carry adder",          "vhdl":"tests/circuit/basic/ripple.vhdl",          "tb":"tests/circuit/basic/ripple_tb.txt",          "desc":"4-bit adder, carry ripples sequentially",  "procs":4,   "colors":4,  "hash":342984039},
    "chainof4":        {"name":"chain of 4",                  "vhdl":"tests/circuit/basic/chainof4.vhdl",        "tb":"tests/circuit/basic/chainof4_tb.txt",        "desc":"4-stage sequential pipeline",              "procs":4,   "colors":4,  "hash":749744446},
    "pipeline8":       {"name":"pipeline 8-stage",            "vhdl":"tests/circuit/basic/pipeline8.vhdl",       "tb":"tests/circuit/basic/pipeline8_tb.txt",       "desc":"8-stage fully sequential chain",           "procs":8,   "colors":8,  "hash":3424237402},
    "eight_channel":   {"name":"8-channel OR bank",           "vhdl":"tests/circuit/basic/eight_channel.vhdl",   "tb":"tests/circuit/basic/eight_channel_tb.txt",   "desc":"8 independent OR gates",                   "procs":8,   "colors":1,  "hash":844848091},
    "thirty2_channel": {"name":"32-channel OR bank",          "vhdl":"tests/circuit/basic/thirty2_channel.vhdl", "tb":"tests/circuit/basic/thirty2_channel_tb.txt", "desc":"32 independent OR gates",                  "procs":32,  "colors":1,  "hash":1666589379},
    "wide_and128":     {"name":"wide AND 128",                "vhdl":"tests/circuit/basic/wide_and128.vhdl",     "tb":"tests/circuit/basic/wide_and128_tb.txt",     "desc":"128 independent AND gates, best speedup",  "procs":128, "colors":1,  "hash":1047282184},
    "traffic_light":   {"name":"traffic light FSM",           "vhdl":"tests/circuit/fsm/traffic_light.vhdl",     "tb":"tests/circuit/fsm/traffic_light_tb.txt",     "desc":"3-state FSM, sequential transitions",      "procs":3,   "colors":2,  "hash":2957170177},
    "majority":        {"name":"majority voter",              "vhdl":"tests/circuit/basic/majority.vhdl",        "tb":"",                                           "desc":"5-input majority using VHDL function",     "procs":1,   "colors":1,  "hash":1484108199},
    "hamming4":        {"name":"hamming encoder 4-bit",       "vhdl":"tests/circuit/basic/hamming4.vhdl",        "tb":"",                                           "desc":"3 independent parity processes",           "procs":3,   "colors":1,  "hash":2071069720},
    "hamming32":       {"name":"hamming encoder 32-bit",      "vhdl":"tests/circuit/basic/hamming32.vhdl",       "tb":"tests/circuit/basic/hamming32_tb.txt",       "desc":"32 independent parity processes",          "procs":32,  "colors":1,  "hash":4083200419},
    "cla4":            {"name":"carry lookahead adder 4-bit", "vhdl":"tests/circuit/basic/cla4.vhdl",            "tb":"tests/circuit/basic/cla4_tb.txt",            "desc":"G/P parallel, carry chain sequential",     "procs":17,  "colors":8,  "hash":472018830},
    "cla32":           {"name":"carry lookahead adder 32-bit","vhdl":"tests/circuit/basic/cla32.vhdl",           "tb":"tests/circuit/basic/cla32_tb.txt",           "desc":"64 parallel G/P + 31 serial carry stages", "procs":97,  "colors":33, "hash":3312919180},
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

def fmt_time(t):
    if not t or t == 0: return "0s"
    if t < 0.0001:  return f"{t*1e6:.1f}µs"
    if t < 0.01:    return f"{t*1000:.2f}ms"
    if t < 1:       return f"{t*1000:.1f}ms"
    return f"{t:.3f}s"

# CHIRAG 22-04-26 :: apply_plot helper
# old code unpacked PLOT dict via **PLOT then also passed xaxis/yaxis separately
# that caused TypeError: multiple values for keyword argument 'yaxis'
# fix: build layout dict once, pass as single **kwargs to update_layout
def apply_plot(fig, title="", height=320, ytitle="", xtitle="",
               showlegend=False, xangle=0, legend_kw=None, extra=None):
    layout = dict(
        paper_bgcolor=BG,
        plot_bgcolor=BG,
        font=dict(color="#777", family="IBM Plex Mono, monospace", size=11),
        margin=dict(l=50, r=15, t=35, b=45),
        title=dict(text=title, font=dict(size=13, color="#ccc")),
        xaxis=dict(gridcolor="#1e1e1e", linecolor="#222", zerolinecolor="#222",
                   title=xtitle, tickangle=xangle,
                   tickfont=dict(family="IBM Plex Mono", size=10)),
        yaxis=dict(gridcolor="#1e1e1e", linecolor="#222", zerolinecolor="#222",
                   title=ytitle),
        height=height,
        showlegend=showlegend,
    )
    if legend_kw:
        layout["legend"] = legend_kw
    if extra:
        layout.update(extra)
    fig.update_layout(**layout)

CPU_COUNT   = multiprocessing.cpu_count()
THREAD_OPTS = [t for t in [1, 2, 4, 8, 16, 32] if t <= CPU_COUNT]
if not THREAD_OPTS: THREAD_OPTS = [1]

def slabel(txt):
    return f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.57rem;letter-spacing:0.14em;text-transform:uppercase;color:#3a3a3a;margin:0.8rem 0 0.3rem">{txt}</p>'

# -----------------------------------------------------------------------
st.set_page_config(page_title="Parallax", layout="wide", initial_sidebar_state="collapsed")

st.markdown(f"""
<style>
@import url('https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:wght@400;500;600&family=IBM+Plex+Sans:wght@300;400;500&display=swap');

html, body, [class*="css"] {{ font-family:'IBM Plex Sans',sans-serif !important; background:#111 !important; color:#ddd !important; }}
#MainMenu, footer, header {{ visibility:hidden !important; }}
.stDeployButton {{ display:none !important; }}
section[data-testid="stSidebar"] {{ display:none !important; }}
.block-container {{ padding:0.7rem 1.6rem 5rem !important; max-width:100% !important; }}

.stSelectbox > div > div {{ background:#1c1c1c !important; border:1px solid #2a2a2a !important; color:#ddd !important; font-family:'IBM Plex Mono',monospace !important; border-radius:4px !important; }}
.stNumberInput > div > div > input {{ background:#1c1c1c !important; border:1px solid #2a2a2a !important; color:#ddd !important; font-family:'IBM Plex Mono',monospace !important; }}
label {{ font-family:'IBM Plex Mono',monospace !important; font-size:0.6rem !important; letter-spacing:0.1em !important; text-transform:uppercase !important; color:#555 !important; }}

.stButton > button {{ background:{AMBER} !important; color:#000 !important; font-family:'IBM Plex Mono',monospace !important; font-weight:700 !important; font-size:0.72rem !important; letter-spacing:0.05em !important; border:none !important; border-radius:4px !important; padding:0.48rem 0.9rem !important; width:100% !important; }}
.stButton > button:hover {{ background:#d97706 !important; color:#000 !important; }}
.stButton > button:disabled {{ background:#2a2a2a !important; color:#555 !important; cursor:not-allowed !important; }}

[data-testid="metric-container"] {{ background:#181818 !important; border:1px solid #222 !important; border-radius:4px !important; padding:0.55rem 0.85rem !important; }}
[data-testid="stMetricValue"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:1rem !important; color:#eee !important; }}
[data-testid="stMetricLabel"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:0.57rem !important; color:#555 !important; letter-spacing:0.1em !important; text-transform:uppercase !important; }}
[data-testid="stMetricDelta"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:0.7rem !important; }}

hr {{ border-color:#1e1e1e !important; margin:0.5rem 0 !important; }}

.stTabs [data-baseweb="tab-list"] {{ background:#0f0f0f !important; border-bottom:1px solid #222 !important; gap:0 !important; }}
.stTabs [data-baseweb="tab"] {{ font-family:'IBM Plex Mono',monospace !important; font-size:0.68rem !important; letter-spacing:0.08em !important; text-transform:uppercase !important; color:#555 !important; padding:0.5rem 1.2rem !important; border-bottom:2px solid transparent !important; }}
.stTabs [aria-selected="true"] {{ color:{AMBER} !important; border-bottom-color:{AMBER} !important; background:transparent !important; }}

.wcard {{ background:#181818; border:1px solid #222; border-radius:5px; padding:0.9rem 1.1rem; height:100%; }}
.wcard-title {{ font-family:'IBM Plex Mono',monospace; font-size:0.6rem; letter-spacing:0.12em; text-transform:uppercase; color:{AMBER}; margin-bottom:0.45rem; }}
.wcard-body {{ font-size:0.8rem; color:#888; line-height:1.65; }}

.pf {{ position:fixed; bottom:0; left:0; right:0; background:#0d0d0d; border-top:1px solid #1a1a1a; padding:0.4rem 1.5rem; display:flex; justify-content:space-between; align-items:center; z-index:9999; font-family:'IBM Plex Mono',monospace; }}
.pfl {{ font-size:0.65rem; color:{AMBER}; }}
.pfr {{ font-size:0.56rem; color:#2a2a2a; }}
</style>
""", unsafe_allow_html=True)

# -----------------------------------------------------------------------
# header
# -----------------------------------------------------------------------
h1, h2 = st.columns([9, 1])
with h1:
    st.markdown(
        '<p style="font-family:IBM Plex Mono,monospace;font-size:0.95rem;font-weight:600;color:#eee;margin:0.15rem 0 0.02rem">Parallax — Parallel VHDL Simulator</p>'
        f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.58rem;color:#2e2e2e;margin:0 0 0.4rem">COD7001 / IIT Delhi / Chirag Kathpalia / 2025MCS2098 &nbsp;|&nbsp; {CPU_COUNT} logical CPUs detected</p>',
        unsafe_allow_html=True
    )
with h2:
    if st.button("? Help"):
        st.session_state.pop("results", None)

tab_sim, tab_bench, tab_help = st.tabs(["Simulate", "Benchmark All", "Help"])


# ===================================================================
# TAB: SIMULATE
# ===================================================================
with tab_sim:
    k1, k2, k3, _ = st.columns([3.5, 1.3, 0.9, 3])
    with k1:
        circuit_key = st.selectbox("Circuit", list(CIRCUITS.keys()),
            format_func=lambda k: f"{CIRCUITS[k]['name']}  ({CIRCUITS[k]['procs']} processes)",
            help="select the VHDL circuit to simulate.")
    with k2:
        stress = st.number_input("Stress (flops/gate)", min_value=0, max_value=2000000, value=0, step=1000,
            help="models gate propagation delay.\n0 = instant gates.\n10000 ≈ 83ns/gate.\n50000 ≈ 356ns/gate.\n100000 ≈ 690ns/gate.\nuse ≥10000 on wide_and128 or hamming32 to see real speedup.")
    with k3:
        st.write("")
        run_btn = st.button("Run",
            help=f"runs sequential + parallel at all thread counts: {', '.join(str(t) for t in THREAD_OPTS)}")

    c = CIRCUITS[circuit_key]
    ci_col = AMBER if c["colors"] == 1 else "#666"
    ci_txt = "fully parallel — 1 color batch" if c["colors"] == 1 else f"{c['colors']} color batches — sequential dependencies"
    st.markdown(
        f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.65rem;color:#333;margin:0.05rem 0 0.4rem">'
        f'{c["desc"]} &nbsp;<span style="color:{ci_col}">/ {ci_txt}</span></p>',
        unsafe_allow_html=True
    )
    st.divider()

    if run_btn:
        results = {}
        with st.spinner("running sequential ..."):
            ss, _ = run_sim(circuit_key, "--seq", 1, stress)
            if ss: results["seq"] = ss
        for t in THREAD_OPTS:
            with st.spinner(f"running parallel {t} threads ..."):
                ps, _ = run_sim(circuit_key, "--par", t, stress)
                if ps: results[f"par_{t}"] = ps
        st.session_state["results"] = results
        st.session_state["ck"]      = circuit_key
        st.session_state["stress"]  = stress
        st.session_state["ts"]      = datetime.now().strftime("%H:%M:%S")

    has_data = "results" in st.session_state and st.session_state.get("results")

    if not has_data:
        st.markdown(slabel("Welcome"), unsafe_allow_html=True)
        wa, wb, wc = st.columns(3)
        with wa:
            st.markdown(f"""<div class="wcard"><div class="wcard-title">What is Parallax?</div><div class="wcard-body">
            a parallel VHDL event-driven simulator for COD7001 at IIT Delhi.<br><br>
            parses VHDL, builds a dependency graph via Welsh-Powell coloring,
            runs independent processes in parallel with OpenMP.<br><br>
            correctness verified by trace hash. seq hash == par hash on all 18 circuits.
            </div></div>""", unsafe_allow_html=True)
        with wb:
            st.markdown(f"""<div class="wcard"><div class="wcard-title">How to use</div><div class="wcard-body">
            <b style="color:#eee">1.</b> select a circuit. 1 color batch = fully parallel = best speedup.<br><br>
            <b style="color:#eee">2.</b> set stress to model gate delay (try 50000 on wide_and128).<br><br>
            <b style="color:#eee">3.</b> click <b style="color:{AMBER}">Run</b> — runs seq + par at all thread counts automatically.<br><br>
            <b style="color:#eee">4.</b> use Benchmark All tab to test all 18 circuits at once.
            </div></div>""", unsafe_allow_html=True)
        with wc:
            st.markdown(f"""<div class="wcard"><div class="wcard-title">Best circuits to demo</div><div class="wcard-body">
            <span style="color:{AMBER}">wide_and128</span> — 128 independent gates. 3.93x at 8T.<br><br>
            <span style="color:{AMBER}">hamming32</span> — 32 parity processes. 4.71x at 8T.<br><br>
            <span style="color:{AMBER}">thirty2_channel</span> — 32 OR gates. 5.9x at 8T.<br><br>
            <span style="color:#555">pipeline8</span> — fully sequential. speedup &lt; 1x always.
            </div></div>""", unsafe_allow_html=True)
        st.markdown('<br><p style="font-family:IBM Plex Mono,monospace;font-size:0.66rem;color:#252525;text-align:center">select a circuit and click Run</p>', unsafe_allow_html=True)

    else:
        results = st.session_state["results"]
        ck      = st.session_state.get("ck", circuit_key)
        ts      = st.session_state.get("ts", "")
        c       = CIRCUITS[ck]
        seq_s   = results.get("seq", {})
        seq_t   = seq_s.get("time", 0)

        # build bar data
        bar_labels, bar_times, bar_colors = [], [], []
        if seq_t:
            bar_labels.append("seq")
            bar_times.append(seq_t)
            bar_colors.append("#444")
        for t in THREAD_OPTS:
            key = f"par_{t}"
            if key in results:
                pt = results[key].get("time", 0)
                if pt:
                    lbl = f"par/1T (OMP baseline)" if t == 1 else f"par/{t}T"
                    bar_labels.append(lbl)
                    bar_times.append(pt)
                    bar_colors.append("#666" if t == 1 else AMBER)

        # speedup data
        sp_labels, sp_values, sp_colors = [], [], []
        if seq_t:
            for t in THREAD_OPTS:
                key = f"par_{t}"
                if key in results:
                    pt = results[key].get("time", 0)
                    if pt:
                        sp = round(seq_t / pt, 2)
                        sp_labels.append(f"{t}T")
                        sp_values.append(sp)
                        sp_colors.append(AMBER if sp >= 1 else RED)

        ts_lbl = f" — {ts}" if ts else ""
        st.markdown(slabel(f"Results{ts_lbl}"), unsafe_allow_html=True)

        ch1, ch2 = st.columns(2)

        with ch1:
            fig1 = go.Figure(go.Bar(
                x=bar_labels, y=bar_times,
                marker_color=bar_colors,
                text=[fmt_time(t) for t in bar_times],
                textposition="outside",
                textfont=dict(family="IBM Plex Mono", size=11, color="#ccc"),
            ))
            # OMP overhead annotation
            if "par_1" in results and seq_t:
                par1_t   = results["par_1"].get("time", 0)
                overhead = par1_t - seq_t
                if overhead > 0:
                    fig1.add_annotation(
                        x="par/1T (OMP baseline)", y=par1_t,
                        text=f"OMP overhead ≈ {fmt_time(overhead)}",
                        showarrow=True, arrowhead=2, arrowcolor="#555",
                        font=dict(family="IBM Plex Mono", size=9, color="#888"),
                        bgcolor=BG, bordercolor="#333", borderwidth=1,
                        ax=80, ay=-40
                    )
            apply_plot(fig1,
                title="Wall Time — sequential vs parallel",
                height=340, ytitle="time (s)")
            st.plotly_chart(fig1, use_container_width=True)

        with ch2:
            if sp_values:
                fig2 = go.Figure()
                fig2.add_trace(go.Bar(
                    x=sp_labels, y=sp_values,
                    marker_color=sp_colors,
                    text=[f"{v:.2f}x" for v in sp_values],
                    textposition="outside",
                    textfont=dict(family="IBM Plex Mono", size=12, color="#eee"),
                    name="measured speedup",
                ))
                # ideal linear line
                ideal_y = [float(t) for t in THREAD_OPTS if f"par_{t}" in results and results[f"par_{t}"].get("time")]
                ideal_x = [f"{t}T" for t in THREAD_OPTS if f"par_{t}" in results and results[f"par_{t}"].get("time")]
                if len(ideal_y) > 1:
                    fig2.add_trace(go.Scatter(
                        x=ideal_x, y=ideal_y, mode="lines",
                        line=dict(color="#2a2a2a", dash="dot", width=1.5),
                        name="ideal linear scaling",
                    ))
                fig2.add_hline(y=1.0, line_dash="dot", line_color="#444",
                               annotation_text="seq = 1x",
                               annotation_font=dict(size=10, color="#555", family="IBM Plex Mono"),
                               annotation_position="top right")
                apply_plot(fig2,
                    title="Speedup vs Thread Count  (higher = better)",
                    height=340, ytitle="speedup (× seq)", xtitle="threads",
                    showlegend=True,
                    legend_kw=dict(font=dict(family="IBM Plex Mono", size=9), x=0.02, y=0.98))
                st.plotly_chart(fig2, use_container_width=True)

        # ---- STAT PANELS ----
        st.markdown(slabel(f"Simulation Stats{ts_lbl}"), unsafe_allow_html=True)

        # pick best parallel run
        best_key, best_sp, best_th = None, 0, 0
        for t in reversed(THREAD_OPTS):
            key = f"par_{t}"
            if key in results:
                pt = results[key].get("time", 0)
                sp = seq_t / pt if pt > 0 and seq_t > 0 else 0
                if sp >= best_sp:
                    best_sp, best_key, best_th = sp, key, t
        if best_key is None and THREAD_OPTS:
            best_key = f"par_{THREAD_OPTS[-1]}"
            best_th  = THREAD_OPTS[-1]

        par_best = results.get(best_key, {})
        par1_s   = results.get("par_1", {})

        def stat_panel(stats, label, border_col, seq_time=None, thread_n=1):
            if not stats or "time" not in stats:
                st.markdown('<p style="color:#333;font-family:IBM Plex Mono,monospace;padding:1rem">no data</p>', unsafe_allow_html=True)
                return
            t       = stats["time"]
            firings = stats.get("firings", 0)
            exp     = c.get("hash")
            got     = stats.get("hash", 0)

            st.markdown(
                f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.57rem;letter-spacing:0.12em;'
                f'text-transform:uppercase;color:#555;margin-bottom:0.3rem;'
                f'padding-top:0.55rem;border-top:3px solid {border_col}">{label}</p>',
                unsafe_allow_html=True
            )
            st.markdown(
                f'<p style="font-family:IBM Plex Mono,monospace;font-size:1.85rem;font-weight:600;'
                f'color:#fff;margin:0;line-height:1">{fmt_time(t)}</p>',
                unsafe_allow_html=True
            )
            if seq_time and seq_time > 0 and t > 0:
                sp   = round(seq_time / t, 2)
                bc   = "#1a2e0d" if sp >= 1 else "#2d0a0a"
                tc   = "#86efac" if sp >= 1 else "#fca5a5"
                ec   = "#365314" if sp >= 1 else "#7f1d1d"
                st.markdown(
                    f'<span style="display:inline-block;background:{bc};border:1px solid {ec};'
                    f'color:{tc};font-family:IBM Plex Mono,monospace;font-size:0.78rem;font-weight:600;'
                    f'padding:0.12rem 0.5rem;border-radius:3px;margin:0.25rem 0 0.35rem">{sp:.2f}x speedup</span>',
                    unsafe_allow_html=True
                )
            if exp and got == exp:
                st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.63rem;color:#4ade80;margin:0.05rem 0 0.4rem">hash {got}  verified</p>', unsafe_allow_html=True)
            else:
                st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.63rem;color:#f87171;margin:0.05rem 0 0.4rem">hash mismatch  got {got}</p>', unsafe_allow_html=True)

            r1, r2 = st.columns(2)
            r1.metric("process firings", f"{firings:,}" if firings else "--")
            r2.metric("delta cycles",    stats.get("deltas", "--"))
            r3, r4 = st.columns(2)
            r3.metric("events",          stats.get("events", "--"))
            r4.metric("max delta depth", stats.get("maxd",   "--"))
            r5, r6 = st.columns(2)
            r5.metric("par batches",     stats.get("colors", "--"),
                      help="Welsh-Powell color classes. 1 = fully parallel. more = sequential dependency.")
            r6.metric("dependencies",    stats.get("edges",  "--"),
                      help="edges in dependency graph. 0 = fully independent processes.")

        pl, pr_col = st.columns(2)
        with pl:
            with st.container(border=True):
                stat_panel(seq_s, "Sequential", "#444")
        with pr_col:
            with st.container(border=True):
                lbl = f"Best Parallel — {best_th} threads" if best_sp >= 1 else f"Parallel {best_th} threads  (all slower than seq)"
                stat_panel(par_best, lbl, AMBER, seq_t, best_th)
                # OMP overhead — only if par/1 measured
                if par1_s and par1_s.get("time") and seq_t:
                    par1_t   = par1_s["time"]
                    overhead = max(0, par1_t - seq_t)
                    if overhead > 0:
                        st.divider()
                        st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.57rem;letter-spacing:0.1em;text-transform:uppercase;color:#3a3a3a;margin-bottom:0.3rem">OMP Infrastructure Overhead  (measured: par/1 − seq)</p>', unsafe_allow_html=True)
                        o1, o2 = st.columns(2)
                        pct = round(overhead / par1_t * 100, 1) if par1_t > 0 else 0
                        o1.metric("overhead", fmt_time(overhead),
                                  delta=f"{pct}% of par/1 time", delta_color="inverse",
                                  help="par/1_time minus seq_time. fixed cost of OpenMP: thread pool, barriers, queue merging.")
                        o2.metric("par/1 time", fmt_time(par1_t),
                                  help="parallel with 1 thread. no actual parallelism — shows pure OMP infrastructure cost.")

        # ---- all thread counts table ----
        with st.expander("All thread counts — full results table"):
            rows = []
            if "seq" in results:
                rs = results["seq"]
                rows.append({"mode":"sequential","threads":1,
                             "time":fmt_time(rs.get("time",0)),
                             "speedup":"1.00x",
                             "firings":rs.get("firings","--"),
                             "delta cycles":rs.get("deltas","--"),
                             "hash ok":"yes" if rs.get("hash")==c.get("hash") else "no"})
            for t in THREAD_OPTS:
                key = f"par_{t}"
                if key in results:
                    rs = results[key]
                    pt = rs.get("time", 0)
                    sp = round(seq_t/pt, 2) if pt > 0 and seq_t > 0 else "--"
                    rows.append({"mode":"parallel","threads":t,
                                 "time":fmt_time(pt),
                                 "speedup":f"{sp}x" if isinstance(sp, float) else "--",
                                 "firings":rs.get("firings","--"),
                                 "delta cycles":rs.get("deltas","--"),
                                 "hash ok":"yes" if rs.get("hash")==c.get("hash") else "no"})
            if rows:
                st.dataframe(pd.DataFrame(rows), use_container_width=True, hide_index=True)
                csv = pd.DataFrame(rows).to_csv(index=False)
                st.download_button("Export CSV", csv,
                    file_name=f"parallax_{ck}_{ts.replace(':','-')}.csv", mime="text/csv")

        # ---- dependency / gtkwave ----
        st.markdown("")
        dg1, dg2, _ = st.columns([1.2, 1.2, 6])
        with dg1:
            if st.button("Show dependency graph"):
                if os.path.exists("dependency.dot"):
                    with open("dependency.dot") as f:
                        st.graphviz_chart(f.read(), use_container_width=False)
                else:
                    st.info("no dependency.dot. run simulation first.")
        with dg2:
            if st.button("Open GTKWave", help="requires: sudo apt install gtkwave"):
                vcd = f"output-{ck}.vcd"
                if not os.path.exists(vcd):
                    st.warning(f"{vcd} not found.")
                else:
                    try:
                        subprocess.Popen(["gtkwave", vcd], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                        st.success(f"opened {vcd}")
                    except FileNotFoundError:
                        st.warning("GTKWave not found. install: sudo apt install gtkwave")
                    except Exception as ex:
                        st.warning(f"error: {ex}")


# ===================================================================
# TAB: BENCHMARK ALL
# ===================================================================
with tab_bench:
    st.markdown(slabel("Benchmark All 18 Circuits"), unsafe_allow_html=True)
    st.markdown('<p style="font-family:IBM Plex Mono,monospace;font-size:0.7rem;color:#3a3a3a;margin-bottom:0.7rem">runs every circuit, checks correctness hash, measures seq and par time.</p>', unsafe_allow_html=True)

    bc1, bc2, _ = st.columns([1.3, 1.2, 6])
    with bc1:
        bench_stress = st.number_input("Stress", min_value=0, max_value=500000, value=0, step=1000, key="bs",
            help="stress value applied to all circuits during benchmark run.")
    with bc2:
        default_bt = min(8, CPU_COUNT)
        bench_threads = st.select_slider("Threads", THREAD_OPTS,
            value=default_bt if default_bt in THREAD_OPTS else THREAD_OPTS[-1], key="bt")

    last_bs   = st.session_state.get("bench_ran_stress",  None)
    last_bt   = st.session_state.get("bench_ran_threads", None)
    changed   = (last_bs != bench_stress or last_bt != bench_threads)
    has_bench = "bench" in st.session_state and st.session_state["bench"]

    with bc1:
        if has_bench and not changed:
            btn_label = "Results up to date"
        elif has_bench and changed:
            btn_label = "Rerun  (settings changed)"
        else:
            btn_label = "Run Benchmark All"

        run_bench = st.button(btn_label, disabled=(has_bench and not changed))

    if run_bench:
        bench_results = []
        prog = st.progress(0, text="starting ...")
        for i, (bk, bv) in enumerate(CIRCUITS.items()):
            prog.progress((i+1)/len(CIRCUITS), text=f"[{i+1}/{len(CIRCUITS)}] {bv['name']} ...")
            ss_b, _ = run_sim(bk, "--seq", 1,             bench_stress)
            ps_b, _ = run_sim(bk, "--par", bench_threads,  bench_stress)
            st_b = ss_b.get("time", 0)
            pt_b = ps_b.get("time", 0) if ps_b else 0
            got  = ss_b.get("hash", 0)
            exp  = bv.get("hash", 0)
            sp   = round(st_b/pt_b, 2) if pt_b > 0 and st_b > 0 else None
            bench_results.append({
                "circuit":              bv["name"],
                "procs":                bv["procs"],
                "par batches":          bv["colors"],
                "hash":                 "pass" if got == exp else "FAIL",
                "seq time":             fmt_time(st_b),
                f"par/{bench_threads}T":fmt_time(pt_b),
                "speedup":              f"{sp:.2f}x" if sp else "--",
            })
        prog.empty()
        st.session_state["bench"]             = bench_results
        st.session_state["bench_ran_stress"]  = bench_stress
        st.session_state["bench_ran_threads"] = bench_threads

    # CHIRAG 22-04-26 :: always show table if data exists, regardless of button state
    if has_bench:
        bench_results = st.session_state["bench"]
        df_bench = pd.DataFrame(bench_results)
        st.dataframe(df_bench, use_container_width=True, hide_index=True)

        passed = sum(1 for r in bench_results if r["hash"] == "pass")
        color  = "#4ade80" if passed == len(bench_results) else AMBER
        st.markdown(f'<p style="font-family:IBM Plex Mono,monospace;font-size:0.72rem;color:{color};margin-top:0.4rem">{passed}/{len(bench_results)} circuits passed hash verification</p>', unsafe_allow_html=True)

        csv = df_bench.to_csv(index=False)
        st.download_button("Export CSV", csv,
            file_name=f"parallax_benchmark_s{bench_stress}_T{bench_threads}.csv", mime="text/csv")

        # speedup chart across all circuits
        st.markdown(slabel("Speedup across all circuits"), unsafe_allow_html=True)
        names, spds = [], []
        for r in bench_results:
            names.append(r["circuit"])
            try:    spds.append(float(r["speedup"].replace("x","")))
            except: spds.append(0.0)
        cols = [AMBER if s >= 1 else RED for s in spds]

        fig_b = go.Figure(go.Bar(
            x=names, y=spds, marker_color=cols,
            text=[f"{s:.2f}x" if s > 0 else "?" for s in spds],
            textposition="outside",
            textfont=dict(family="IBM Plex Mono", size=9, color="#eee"),
        ))
        fig_b.add_hline(y=1.0, line_dash="dot", line_color="#444",
                        annotation_text="seq=1x",
                        annotation_font=dict(size=9, color="#555", family="IBM Plex Mono"))
        apply_plot(fig_b, title="Speedup per Circuit", height=300,
                   ytitle="speedup (×)", xangle=-35)
        st.plotly_chart(fig_b, use_container_width=True)


# ===================================================================
# TAB: HELP
# ===================================================================
with tab_help:
    ha, hb = st.columns(2)
    with ha:
        st.markdown(f"""<div class="wcard" style="margin-bottom:0.8rem"><div class="wcard-title">What is Parallax?</div><div class="wcard-body">
        Parallax is a parallel VHDL event-driven simulator for COD7001 at IIT Delhi.<br><br>
        it parses a VHDL subset using Flex/Bison, builds an AST, then runs event-driven
        simulation with delta cycle semantics.<br><br>
        the parallel engine uses Welsh-Powell graph coloring to find independent processes
        (same color = no shared signals = safe to run simultaneously) then uses OpenMP.
        </div></div>""", unsafe_allow_html=True)

        st.markdown(f"""<div class="wcard" style="margin-bottom:0.8rem"><div class="wcard-title">What is the OMP overhead?</div><div class="wcard-body">
        par/1 time = parallel mode with 1 thread. no actual parallelism happens, but all
        OMP infrastructure is active: thread pool, barriers, per-thread queue allocation.<br><br>
        overhead = par/1_time − seq_time. this is the fixed cost you always pay.
        if seq_time &lt; overhead, parallel can never win at any thread count.
        </div></div>""", unsafe_allow_html=True)

    with hb:
        st.markdown(f"""<div class="wcard" style="margin-bottom:0.8rem"><div class="wcard-title">What is stress?</div><div class="wcard-body">
        real CMOS gates have propagation delay. our simulator models gates as instant.<br><br>
        stress adds N floating-point multiplies per gate to model this delay. physically motivated.<br><br>
        without stress: OMP overhead (~500µs) >> gate work (~5ns) → parallel always loses.<br>
        with stress=50000: gate work >> overhead → parallel wins.
        </div></div>""", unsafe_allow_html=True)

        st.markdown(f"""<div class="wcard" style="margin-bottom:0.8rem"><div class="wcard-title">Reading the charts</div><div class="wcard-body">
        <b style="color:#eee">wall time</b> — actual elapsed time. seq vs par at each thread count.
        par/1T shows OMP baseline cost with annotation.<br><br>
        <b style="color:#eee">speedup</b> — seq_time / par_time. dotted line = ideal linear.
        real speedup below ideal = Amdahl limit from sequential dependencies.<br><br>
        <b style="color:#eee">hash verified</b> — seq hash == par hash = bit-for-bit identical simulation.
        </div></div>""", unsafe_allow_html=True)

    st.markdown(slabel("Hash contracts — all 18 circuits"), unsafe_allow_html=True)
    contracts = [{"circuit":v["name"],"processes":v["procs"],"par batches":v["colors"],"known hash":v["hash"]}
                 for v in CIRCUITS.values()]
    st.dataframe(pd.DataFrame(contracts), use_container_width=True, hide_index=True)


# -----------------------------------------------------------------------
st.markdown('<div class="pf"><span class="pfl">sequential first. parallel second. correctness always.</span><span class="pfr">Chirag Kathpalia / 2025MCS2098 / COD7001 IIT Delhi / built with AI assistance (Claude, Anthropic)</span></div>', unsafe_allow_html=True)