#pragma once

#include <Arduino.h>

const char SONAR_WEB_PAGE[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Sonar-Radar</title>
  <style>
    :root { color-scheme: dark; --cyan:#53f6e3; --muted:#7ea5a9; --panel:#071a23; }
    * { box-sizing:border-box; }
    body { margin:0; min-height:100vh; font-family:ui-monospace,Consolas,monospace; color:#d8fffb;
      background:radial-gradient(circle at 50% 0%,#123b4a 0,#06131c 48%,#020609 100%); }
    main { width:min(960px,100%); margin:auto; padding:22px 16px 32px; }
    header { display:flex; justify-content:space-between; align-items:center; gap:12px; margin-bottom:16px; }
    h1 { margin:0; font-size:clamp(1.05rem,4vw,1.7rem); letter-spacing:.08em; }
    .live { color:var(--cyan); font-size:.75rem; white-space:nowrap; }
    .dot { display:inline-block; width:9px; height:9px; border-radius:99px; background:var(--cyan); box-shadow:0 0 12px var(--cyan); margin-right:6px; }
    .scope { border:1px solid #28616b; border-radius:14px; padding:10px; background:linear-gradient(145deg,#0c2d37,#031016); box-shadow:0 20px 50px #0008; }
    canvas { width:100%; display:block; border-radius:8px; background:radial-gradient(ellipse at 50% 100%,#0b4550,#04141c 63%,#02080c); }
    .stats { display:grid; grid-template-columns:repeat(4,minmax(0,1fr)); gap:9px; margin-top:14px; }
    .stat { border:1px solid #20515a; border-radius:9px; padding:10px; background:var(--panel); min-width:0; }
    .stat span { display:block; color:var(--muted); font-size:.65rem; letter-spacing:.08em; }
    .stat strong { display:block; margin-top:5px; color:var(--cyan); overflow:hidden; text-overflow:ellipsis; white-space:nowrap; font-size:.94rem; }
    .foot { color:#71949b; text-align:center; margin:15px 0 0; font-size:.68rem; }
    @media (max-width:560px) { .stats { grid-template-columns:repeat(2,1fr); } main { padding:15px 10px; } }
  </style>
</head>
<body>
  <main>
    <header><h1>U-BOOT SONAR // RADARSTATION</h1><div class="live"><i class="dot"></i>LIVE</div></header>
    <section class="scope"><canvas id="scope" width="900" height="540" aria-label="Sonar-Scope"></canvas></section>
    <section class="stats">
      <div class="stat"><span>SCAN</span><strong id="scan">--</strong></div>
      <div class="stat"><span>SUCHWINKEL</span><strong id="angle">--</strong></div>
      <div class="stat"><span>ECHOS</span><strong id="echoes">--</strong></div>
      <div class="stat"><span>NAECHSTER KONTAKT</span><strong id="closest">--</strong></div>
    </section>
    <p class="foot">0-90 DEG SEKTORSCANNING &middot; REICHWEITE 0-300 CM &middot; ESP32-S3</p>
  </main>
  <script>
    const canvas = document.querySelector('#scope'), ctx = canvas.getContext('2d');
    let snapshot = { scan:0, angle:0, forward:true, echoes:0, closest:0, samples:[] };
    // Eine Sonarspur bleibt kurz sichtbar. Das macht einzelne, echte Echos
    // auch zwischen zwei HTTP-Aktualisierungen deutlich erkennbar.
    const traceMemory = Array.from({length:61}, () => ({distance:0, seen:0}));
    const traceLifetimeMs = 6000;
    const cyan = '#53f6e3';
    function point(deg, distance, cx, cy, radius) {
      const theta = (-135 + deg) * Math.PI / 180, r = radius * distance / 300;
      return [cx + Math.cos(theta) * r, cy + Math.sin(theta) * r];
    }
    function draw() {
      const w = canvas.width, h = canvas.height, cx = w / 2, cy = h * .91, r = Math.min(w * .43, h * .79);
      ctx.clearRect(0,0,w,h); ctx.lineWidth = 1; ctx.font = '13px ui-monospace, monospace';
      ctx.strokeStyle = '#1b6872'; ctx.fillStyle = '#74aeb2';
      for (let meters=50; meters<=300; meters+=50) { ctx.beginPath(); ctx.arc(cx,cy,r*meters/300,-3*Math.PI/4,-Math.PI/4); ctx.stroke();
        ctx.fillText(meters+' cm', cx+5, cy-r*meters/300-5); }
      for (let deg=0; deg<=90; deg+=15) { const p=point(deg,300,cx,cy,r); ctx.beginPath(); ctx.moveTo(cx,cy); ctx.lineTo(...p); ctx.stroke();
        ctx.fillText(deg+'', p[0]-7, p[1]-9); }
      ctx.strokeStyle = '#49c6bd'; ctx.lineWidth = 2; ctx.beginPath(); ctx.arc(cx,cy,r,-3*Math.PI/4,-Math.PI/4); ctx.stroke();
      const now = Date.now();
      traceMemory.forEach((trace,index) => { if (!trace.distance) return; const age=now-trace.seen, alpha=1-age/traceLifetimeMs;
        if (alpha<=0) return; const p=point(index*1.5,trace.distance,cx,cy,r), size=2.5+trace.distance/130;
        ctx.fillStyle='rgba(83,246,227,'+(0.10+alpha*.44)+')'; ctx.shadowColor=cyan; ctx.shadowBlur=5+alpha*9;
        ctx.beginPath(); ctx.arc(p[0],p[1],size,0,Math.PI*2); ctx.fill(); ctx.shadowBlur=0;
      });
      snapshot.samples.forEach((distance,index) => { if (!distance) return; const p=point(index*1.5,distance,cx,cy,r);
        ctx.fillStyle='#d8fffb'; ctx.shadowColor=cyan; ctx.shadowBlur=14;
        ctx.beginPath(); ctx.arc(p[0],p[1],3+distance/150,0,Math.PI*2); ctx.fill(); ctx.shadowBlur=0; });
      const sweep=point(snapshot.angle,300,cx,cy,r); ctx.strokeStyle='#d8fffb'; ctx.globalAlpha=.72; ctx.beginPath(); ctx.moveTo(cx,cy); ctx.lineTo(...sweep); ctx.stroke(); ctx.globalAlpha=1;
      ctx.fillStyle=cyan; ctx.shadowColor=cyan; ctx.shadowBlur=14; ctx.beginPath(); ctx.arc(cx,cy,5,0,Math.PI*2); ctx.fill(); ctx.shadowBlur=0;
      ctx.fillStyle='#a7d4d5'; ctx.fillText(snapshot.forward ? 'VORWAERTS-SCAN' : 'RUECKLAUF-SCAN', 18, 26);
    }
    function show(data) {
      snapshot=data;
      const seen=Date.now();
      data.samples.forEach((distance,index) => { if (distance) traceMemory[index]={distance,seen}; });
      draw();
      document.querySelector('#scan').textContent = '#'+data.scan;
      document.querySelector('#angle').textContent = data.angle+' DEG '+(data.forward ? '>' : '<');
      document.querySelector('#echoes').textContent = data.echoes;
      document.querySelector('#closest').textContent = data.closest ? data.closest+' cm' : 'KEIN ECHO';
    }
    async function refresh() { try { const response=await fetch('/api/scan',{cache:'no-store'}); show(await response.json()); } catch (_) { document.querySelector('.live').innerHTML='OFFLINE'; } }
    refresh(); setInterval(refresh,400); window.addEventListener('resize',draw);
  </script>
</body>
</html>
)rawliteral";
