// ============================================================
//  FOODBRIDGE — app.js
//  Full frontend logic connecting to C++ backend API
// ============================================================

// ── STATE ─────────────────────────────────────────────────
let ngos = [];
let donations = [];
let allocationResults = [];
let totalFoodStock = 500;
let allocationCount = 0;
let currentUser = null;
let currentRole = 'admin';

// ── MAP VARIABLES ─────────────────────────────────────────
let map;
let mapMarkers = [];
let currentUserLocation = null;
let newlyAddedNGOIds = [];

// ── DEMO USERS ─────────────────────────────────────────────
const USERS = {
  'admin@foodbridge.org': { password: 'admin123', role: 'admin',  name: 'Admin User',        avatar: 'A', roleLabel: 'System Administrator' },
  'hostel@mess.edu':      { password: 'hostel123', role: 'hostel', name: 'Hostel Manager',    avatar: 'H', roleLabel: 'Hostel / Mess Supplier' },
  'ngo@help.org':         { password: 'ngo123',    role: 'ngo',    name: 'NGO Coordinator',   avatar: 'N', roleLabel: 'NGO Representative' }
};

// ==================== VALID DEHRADUN NGO LOCATIONS ====================
const validNGOLocations = [
    { name: "Pratishtha Foundation", lat: 30.2950, lng: 78.0180, urgency: 9, beneficiaries: 450 },
    { name: "Balajee Sewa Sansthan", lat: 30.2975, lng: 78.0220, urgency: 8, beneficiaries: 380 },
    { name: "Health Captain NGO", lat: 30.2980, lng: 78.0240, urgency: 7, beneficiaries: 200 },
    { name: "Waste Warrior NGO", lat: 30.2920, lng: 78.0150, urgency: 6, beneficiaries: 180 },
    { name: "Mindrolling Monastery", lat: 30.2930, lng: 78.0210, urgency: 5, beneficiaries: 300 },
    { name: "Graphic Era Society", lat: 30.2910, lng: 78.0160, urgency: 8, beneficiaries: 500 },
    { name: "U H Foundation", lat: 30.2960, lng: 78.0190, urgency: 7, beneficiaries: 320 },
    { name: "Aashray Welfare Society", lat: 30.2990, lng: 78.0250, urgency: 7, beneficiaries: 280 },
    { name: "Roti Bank Dehradun", lat: 30.3000, lng: 78.0200, urgency: 9, beneficiaries: 600 },
    { name: "Seva Sankalp", lat: 30.2940, lng: 78.0230, urgency: 7, beneficiaries: 250 }
];

// ── HELPER FUNCTIONS ───────────────────────────────────────
function getNGOLocation(name) {
    const found = validNGOLocations.find(loc => 
        loc.name.toLowerCase() === name.toLowerCase()
    );
    if (found) {
        return { lat: found.lat, lng: found.lng, urgency: found.urgency, beneficiaries: found.beneficiaries };
    }
    return null;
}

// ── SORTING HELPER ─────────────────────────────────────────
function sortNGOsByPriority(data) {
  return [...data].sort((a, b) => {
    const scoreA = a.priorityScore || 0;
    const scoreB = b.priorityScore || 0;
    return scoreB - scoreA;
  });
}

// ── INIT ───────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  updateClock();
  setInterval(updateClock, 1000);
  setInterval(loadFoodStock, 10000);
  checkBackendOnLoad();
});

function updateClock() {
  const el = document.getElementById('time-display');
  if (el) el.textContent = new Date().toLocaleTimeString();
}

// ── BACKEND CONNECTION CHECK ────────────────────────────────
async function checkBackendOnLoad() {
  try {
    const res = await fetch('/api/food-stock');
    if (!res.ok) throw new Error();
    showBannerToast('🟢 Backend connected successfully!', 'success');
  } catch {
    showBannerToast('🔴 Backend not reachable. Make sure the server is running.', 'error');
  }
}

async function checkBackendAfterLogin() {
  try {
    const res = await fetch('/api/food-stock');
    if (!res.ok) throw new Error();
    showToast('🟢 Backend connected successfully!', 'success');
  } catch {
    showToast('🔴 Backend not reachable. Some features may not work.', 'error');
  }
}

function showBannerToast(msg, type = 'info') {
  const existing = document.getElementById('banner-toast');
  if (existing) existing.remove();

  const colors = {
    success: { bg: '#1a3a2a', border: '#2ecc71', color: '#2ecc71' },
    error:   { bg: '#3a1a1a', border: '#e74c3c', color: '#e74c3c' },
    info:    { bg: '#1a2a3a', border: '#4f8ef7', color: '#4f8ef7' }
  };
  const c = colors[type] || colors.info;

  const banner = document.createElement('div');
  banner.id = 'banner-toast';
  banner.textContent = msg;
  banner.style.cssText = `
    position: fixed;
    top: 18px;
    left: 50%;
    transform: translateX(-50%);
    background: ${c.bg};
    color: ${c.color};
    border: 1.5px solid ${c.border};
    border-radius: 10px;
    padding: 12px 28px;
    font-size: 14px;
    font-weight: 600;
    z-index: 99999;
    box-shadow: 0 4px 24px rgba(0,0,0,0.4);
    letter-spacing: 0.3px;
    transition: opacity 0.4s, transform 0.4s;
  `;
  document.body.appendChild(banner);

  setTimeout(() => {
    banner.style.opacity = '0';
    banner.style.transform = 'translateX(-50%) translateY(-10px)';
    setTimeout(() => banner.remove(), 400);
  }, 4000);
}

// ── AUTH ───────────────────────────────────────────────────
function selectRole(role, btn) {
  currentRole = role;
  document.querySelectorAll('.role-btn').forEach(b => b.classList.remove('active'));
  btn.classList.add('active');
  const demos = { admin: ['admin@foodbridge.org','admin123'], hostel: ['hostel@mess.edu','hostel123'], ngo: ['ngo@help.org','ngo123'] };
  fillDemo(...demos[role]);
}

function fillDemo(email, pass) {
  document.getElementById('login-email').value = email;
  document.getElementById('login-password').value = pass;
}

function togglePassword() {
  const inp = document.getElementById('login-password');
  inp.type = inp.type === 'password' ? 'text' : 'password';
}

function handleLogin() {
  const email = document.getElementById('login-email').value.trim();
  const pass  = document.getElementById('login-password').value;

  if (!email || !pass) { showToast('Please enter email and password', 'error'); return; }

  const user = USERS[email];
  if (!user || user.password !== pass) {
    showToast('Invalid credentials. Try demo accounts below.', 'error'); return;
  }

  currentUser = { email, ...user };
  document.getElementById('user-name').textContent   = user.name;
  document.getElementById('user-role').textContent   = user.roleLabel;
  document.getElementById('user-avatar').textContent = user.avatar;

  document.getElementById('login-page').classList.remove('active');
  document.getElementById('main-page').classList.add('active');

  loadNGOs();
  loadFoodStock();
  loadDonations();
  checkBackendAfterLogin();
  showToast(`Welcome back, ${user.name}! 👋`, 'success');
}

function handleLogout() {
  currentUser = null;
  document.getElementById('main-page').classList.remove('active');
  document.getElementById('login-page').classList.add('active');
  showToast('Signed out successfully', 'info');
}

// ── NAVIGATION ─────────────────────────────────────────────
const PAGE_TITLES = {
  dashboard:  'Dashboard',
  ngo:        'NGO Management',
  food:       'Food Donations',
  allocation: 'Allocation Engine',
  reports:    'Reports',
  complexity: 'Complexity Analysis'
};

function switchTab(tab, btn) {
  document.querySelectorAll('.nav-item').forEach(b => b.classList.remove('active'));
  if (btn) btn.classList.add('active');

  document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));
  const pane = document.getElementById(`${tab}-tab`);
  if (pane) pane.classList.add('active');

  document.getElementById('page-title').textContent = PAGE_TITLES[tab] || tab;
  document.getElementById('breadcrumb').textContent = `Home / ${PAGE_TITLES[tab] || tab}`;

  if (tab === 'dashboard') {
    refreshDashboard();
    setTimeout(() => {
      if (document.getElementById('food-map')) {
        initFoodMap();
        updateMapMarkers();
      }
    }, 200);
  }
}

function toggleSidebar() {
  document.getElementById('sidebar').classList.toggle('collapsed');
}

// ── API: LOAD NGOs ──────────────────────────────────────────
async function loadNGOs() {
  try {
    const res = await fetch('/api/ngos');
    if (!res.ok) throw new Error();
    ngos = await res.json();
    ngos = sortNGOsByPriority(ngos);
    displayNGOs(ngos);
    refreshDashboard();
    
    setTimeout(() => {
      if (document.getElementById('food-map')) {
        initFoodMap();
        updateMapMarkers();
      }
    }, 500);
  } catch {
    displayNGOs([]);
  }
}

// ── API: FOOD STOCK ─────────────────────────────────────────
async function loadFoodStock() {
  try {
    const res = await fetch('/api/food-stock');
    if (!res.ok) throw new Error();
    const data = await res.json();
    totalFoodStock = data.foodStock;
    updateStockDisplays(totalFoodStock);
  } catch { /* silent */ }
}

function updateStockDisplays(val) {
  ['food-stock-display','topbar-stock','dash-food-stock'].forEach(id => {
    const el = document.getElementById(id);
    if (el) el.textContent = val;
  });
}

// ── API: LOAD DONATIONS ─────────────────────────────────────
async function loadDonations() {
  try {
    const res = await fetch('/api/donations?limit=50');
    if (!res.ok) throw new Error();
    donations = await res.json();
    displayDonations();
  } catch { /* silent */ }
}

// ── API: ADD NGO - ONLY REAL NGOs ALLOWED ────────────────────
async function addNGO() {
  const name = document.getElementById('ngo-name').value.trim();
  let urgency = parseInt(document.getElementById('ngo-urgency').value);
  let beneficiaries = parseInt(document.getElementById('ngo-beneficiaries').value);
  let quantity = parseInt(document.getElementById('ngo-quantity').value);

  if (!name) { showToast('NGO name is required', 'error'); return; }
  
  const location = getNGOLocation(name);
  
  if (!location) {
    const ngoList = validNGOLocations.map(l => `• ${l.name}`).join('\n');
    showToast(`❌ "${name}" is not a registered NGO in Dehradun.\n\nValid NGOs:\n${ngoList}`, 'error');
    return;
  }
  
  urgency = urgency || location.urgency;
  beneficiaries = beneficiaries || location.beneficiaries;
  quantity = quantity || Math.floor(beneficiaries * 0.5);
  
  if (urgency < 1 || urgency > 10) { showToast('Urgency must be 1–10', 'error'); return; }
  if (beneficiaries < 50) { showToast('Beneficiaries must be at least 50', 'error'); return; }

  try {
    const res = await fetch('/api/ngos', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ 
        name, urgency, beneficiaries, quantity,
        latitude: location.lat, longitude: location.lng
      })
    });
    if (!res.ok) throw new Error();
    hideAddNGOForm();
    await loadNGOs();
    
    // Mark as newly added
    if (ngos.length > 0) {
      const newNGO = ngos[ngos.length - 1];
      newlyAddedNGOIds.push(newNGO.id);
      updateMapMarkers();
      
      setTimeout(() => {
        newlyAddedNGOIds = newlyAddedNGOIds.filter(id => id !== newNGO.id);
        updateMapMarkers();
      }, 10000);
    }
    
    setTimeout(() => {
      zoomToLocation(location.lat, location.lng, name);
    }, 500);
    
    showToast(`✅ ${name} registered successfully!`, 'success');
  } catch {
    showToast('Failed to add NGO. Check backend connection.', 'error');
  }
}

// ── API: CALCULATE PRIORITIES ───────────────────────────────
async function calculatePriorities() {
  try {
    const res = await fetch('/api/calculate-priority', { method: 'POST' });
    if (!res.ok) throw new Error();
    ngos = await res.json();
    ngos = sortNGOsByPriority(ngos);
    displayNGOs(ngos);
    refreshDashboard();
    updateMapMarkers();
    showToast('✅ Priority scores calculated using C++ algorithm!', 'success');
  } catch {
    showToast('Failed to calculate priorities', 'error');
  }
}

// ── API: ADD FOOD DONATION ──────────────────────────────────
async function addFoodDonation() {
  const supplier = document.getElementById('supplier-name').value.trim();
  const foodType = document.getElementById('food-type').value.trim() || 'Mixed Food';
  const quantity = parseInt(document.getElementById('food-quantity').value);

  if (!supplier) { showToast('Supplier name is required', 'error'); return; }
  if (!quantity || quantity < 1) { showToast('Enter a valid quantity', 'error'); return; }

  try {
    const res = await fetch('/api/add-food', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ quantity, supplier, foodType })
    });
    if (!res.ok) throw new Error();
    const data = await res.json();
    totalFoodStock = data.foodStock;
    updateStockDisplays(totalFoodStock);
  } catch {
    totalFoodStock += quantity;
    updateStockDisplays(totalFoodStock);
  }

  hideAddFoodForm();
  await loadDonations();
  document.getElementById('allocation-food-stock').value = totalFoodStock;
  showToast(`✅ Added ${quantity} kg from ${supplier}`, 'success');
}

// ── API: RUN ALLOCATION ─────────────────────────────────────
async function runAllocation() {
  const algorithm = parseInt(document.getElementById('sort-algorithm').value);
  let foodStock   = parseInt(document.getElementById('allocation-food-stock').value);
  if (isNaN(foodStock) || foodStock <= 0) { showToast('Enter a valid food stock value', 'error'); return; }

  const btn = document.querySelector('.run-btn');
  if (btn) { btn.textContent = '⏳ Running...'; btn.disabled = true; }

  try {
    const res = await fetch('/api/allocate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ algorithm, food_stock: foodStock })
    });
    if (!res.ok) throw new Error();
    const data = await res.json();

    allocationResults = data.results || [];
    allocationCount++;

    displayAllocationResults(allocationResults);
    totalFoodStock = data.remainingFood ?? (foodStock - (data.totalAllocated || 0));
    updateStockDisplays(totalFoodStock);

    const bar = document.getElementById('allocation-summary-bar');
    if (bar) bar.style.display = 'flex';
    setText('sum-algorithm',  data.algorithm        || getAlgorithmName(algorithm));
    setText('sum-time',       data.timeComplexity   || getTimeComplexity(algorithm));
    setText('sum-allocated',  (data.totalAllocated  || 0) + ' kg');
    setText('sum-remaining',  (data.remainingFood   || 0) + ' kg');
    setText('sum-efficiency', ((data.efficiency     || 0)).toFixed(1) + '%');

    updateLiveComplexity(data, algorithm);
    refreshDashboard();
    showToast(`⚡ Allocation complete! ${data.totalAllocated || 0} kg distributed`, 'success');
  } catch {
    showToast('Allocation failed. Check backend.', 'error');
  } finally {
    if (btn) { btn.textContent = '⚡ Run Greedy Allocation'; btn.disabled = false; }
  }
}

// ── API: DELETE NGO ─────────────────────────────────────────
async function deleteNGO(id) {
  if (!confirm('Delete this NGO?')) return;
  try {
    const res = await fetch(`/api/ngos/${id}`, { method: 'DELETE' });
    if (!res.ok) throw new Error();
    await loadNGOs();
    showToast('NGO deleted', 'warning');
  } catch {
    showToast('Failed to delete NGO', 'error');
  }
}

// ── FRONTEND ALGORITHMS ─────────────────────────────────────
let sortCol = 'id', sortDir = 'asc';
function sortByColumn(col) {
  if (sortCol === col) sortDir = sortDir === 'asc' ? 'desc' : 'asc';
  else { sortCol = col; sortDir = 'desc'; }
  const sorted = [...ngos].sort((a, b) => {
    let av = a[col], bv = b[col];
    if (typeof av === 'string') { av = av.toLowerCase(); bv = bv.toLowerCase(); }
    if (sortDir === 'desc') return av > bv ? -1 : av < bv ? 1 : 0;
    return av < bv ? -1 : av > bv ? 1 : 0;
  });
  displayNGOs(sorted);
}

function filterNGOs(term) {
  if (!term.trim()) { displayNGOs(ngos); return; }
  const t = term.toLowerCase();
  const filtered = ngos.filter(n =>
    n.name.toLowerCase().includes(t) || String(n.id).includes(t)
  );
  displayNGOs(filtered);
}

function calcStats() {
  if (!ngos.length) return null;
  let totB = 0, totQ = 0, maxU = 0;
  for (const n of ngos) { totB += n.beneficiaries; totQ += n.quantity; if (n.urgency > maxU) maxU = n.urgency; }
  return { count: ngos.length, totalBeneficiaries: totB, totalRequested: totQ, avgB: (totB/ngos.length).toFixed(0), avgQ: (totQ/ngos.length).toFixed(0), maxUrgency: maxU };
}

function getPriorityColor(score) {
  if (!score) return { bg: 'rgba(255,255,255,0.05)', color: '#7b82a0' };
  if (score >= 800) return { bg: 'rgba(231,76,60,0.15)',  color: '#e74c3c' };
  if (score >= 500) return { bg: 'rgba(245,166,35,0.15)', color: '#f5a623' };
  if (score >= 300) return { bg: 'rgba(79,142,247,0.15)', color: '#4f8ef7' };
  return { bg: 'rgba(46,204,113,0.15)', color: '#2ecc71' };
}

function binarySearchNGO(id) {
  const sorted = [...ngos].sort((a,b) => a.id - b.id);
  let lo = 0, hi = sorted.length - 1;
  while (lo <= hi) {
    const mid = (lo + hi) >> 1;
    if (sorted[mid].id === id) return sorted[mid];
    sorted[mid].id < id ? lo = mid + 1 : hi = mid - 1;
  }
  return null;
}

// ── DISPLAY FUNCTIONS ───────────────────────────────────────
function displayNGOs(data) {
  const tbody = document.getElementById('ngo-tbody');
  if (!tbody) return;
  if (!data || !data.length) {
    tbody.innerHTML = '<tr><td colspan="7" class="empty-row">No NGOs registered yet. Click "Add NGO" to get started.</td></tr>';
    return;
  }
  const sortedData = sortNGOsByPriority(data);
  tbody.innerHTML = sortedData.map(n => {
    const c = getPriorityColor(n.priorityScore);
    const pill = n.priorityScore
      ? `<span class="priority-pill" style="background:${c.bg};color:${c.color}">${n.priorityScore}</span>`
      : '<span style="color:var(--muted)">—</span>';
    const urgencyBar = `<span style="font-weight:600">${n.urgency}</span><span style="color:var(--muted);font-size:11px">/10</span>`;
    return `
      <tr>
        <td style="font-family:var(--font-mono);color:var(--muted)">#${n.id}</td>
        <td style="font-weight:500">${n.name}</td>
        <td>${urgencyBar}</td>
        <td>${n.beneficiaries.toLocaleString()}</td>
        <td>${n.quantity} kg</td>
        <td>${pill}</td>
        <td><button class="del-btn" onclick="deleteNGO(${n.id})">🗑 Delete</button></td>
      </tr>
    `;
  }).join('');
}

function displayDonations() {
  const tbody = document.getElementById('donation-tbody');
  if (!tbody) return;
  const el = document.getElementById('donation-count');
  if (el) el.textContent = `${donations.length} entr${donations.length === 1 ? 'y' : 'ies'}`;

  if (!donations.length) {
    tbody.innerHTML = '<tr><td colspan="5" class="empty-row">No donations recorded yet</td></tr>';
    return;
  }
  tbody.innerHTML = donations.slice(0, 20).map(d => `
    <tr>
      <td style="font-family:var(--font-mono);font-size:12px;color:var(--muted)">${d.date}</td>
      <td style="font-weight:500">${d.supplier}</td>
      <td style="color:var(--muted)">${d.foodType}</td>
      <td style="font-family:var(--font-mono);color:var(--accent)">${d.quantity} kg</td>
      <td><span class="status-pill status-full">${d.status || 'Received'}</span></td>
    </tr>
  `).join('');
}

function displayAllocationResults(results) {
  const tbody = document.getElementById('allocation-tbody');
  if (!tbody) return;
  if (!results || !results.length) {
    tbody.innerHTML = '<tr><td colspan="6" class="empty-row">Run allocation to see results</td></tr>';
    return;
  }
  tbody.innerHTML = results.map((r, i) => {
    const statusClass = r.status === 'FULL' ? 'status-full' : r.status === 'PARTIAL' ? 'status-partial' : 'status-none';
    const pct = r.requested > 0 ? Math.round((r.allocated / r.requested) * 100) : 0;
    return `
      <tr>
        <td style="font-family:var(--font-mono);color:var(--muted)">#${i+1}</td>
        <td style="font-weight:500">${r.ngoName}</td>
        <td>${r.priorityScore ? `<span class="priority-pill" style="background:rgba(245,166,35,.15);color:var(--accent)">${r.priorityScore}</span>` : '—'}</td>
        <td>${r.requested} kg</td>
        <td style="font-weight:600">${r.allocated} kg <span style="font-size:11px;color:var(--muted)">(${pct}%)</span></td>
        <td><span class="status-pill ${statusClass}">${r.status}</span></td>
      </tr>
    `;
  }).join('');
}

function refreshDashboard() {
  const stats = calcStats();
  setText('dash-ngo-count',    stats ? stats.count : 0);
  setText('dash-food-stock',   totalFoodStock);
  setText('dash-beneficiaries', stats ? stats.totalBeneficiaries.toLocaleString() : 0);
  setText('dash-allocations',  allocationCount);

  const list = document.getElementById('dash-priority-list');
  if (!list) return;
  const top = [...ngos].filter(n => n.priorityScore).sort((a,b) => b.priorityScore - a.priorityScore).slice(0, 5);
  if (!top.length) {
    list.innerHTML = '<div style="padding:20px;text-align:center;color:var(--muted);font-size:13px">Calculate priority scores first</div>';
    return;
  }
  list.innerHTML = top.map(n => {
    const cls = n.priorityScore >= 800 ? 'pi-high' : n.priorityScore >= 500 ? 'pi-mid' : 'pi-low';
    return `<div class="priority-item"><span class="pi-name">${n.name}</span><span class="pi-score ${cls}">${n.priorityScore}</span></div>`;
  }).join('');
}

function generateReport() {
  const stats = calcStats();
  const now   = new Date().toLocaleString();

  let txt = `╔══════════════════════════════════════════════════════════════╗
║         FOODBRIDGE — NGO FOOD SUPPLIER SYSTEM REPORT         ║
╚══════════════════════════════════════════════════════════════╝

📅  Generated : ${now}
👤  User      : ${currentUser?.name || 'Unknown'} (${currentUser?.roleLabel || ''})

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 PROJECT OVERVIEW
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Name      : NGO Food Supplier System
  Algorithm : Greedy Allocation + Priority-Based Sorting
  Backend   : C++ (cpp-httplib)
  Frontend  : Vanilla JS + FoodBridge UI
  Formula   : Priority = (Urgency × 60) + (Beneficiaries × 40)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 STATISTICS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Total NGOs            : ${stats?.count || 0}
  Total Beneficiaries   : ${stats?.totalBeneficiaries?.toLocaleString() || 0}
  Total Food Requested  : ${stats?.totalRequested || 0} kg
  Current Food Stock    : ${totalFoodStock} kg
  Allocations Run       : ${allocationCount}

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 REGISTERED NGOs
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
`;
  const sortedNGOs = sortNGOsByPriority(ngos);
  for (const n of sortedNGOs) {
    txt += `
  • ${n.name}
    ├─ ID           : ${n.id}
    ├─ Urgency      : ${n.urgency}/10
    ├─ Beneficiaries: ${n.beneficiaries}
    ├─ Requested    : ${n.quantity} kg
    └─ Priority     : ${n.priorityScore || 'Not Calculated'}`;
  }

  txt += `

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 LAST ALLOCATION RESULTS
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
`;
  if (allocationResults.length) {
    for (const r of allocationResults) {
      txt += `
  • ${r.ngoName}
    ├─ Priority  : ${r.priorityScore}
    ├─ Requested : ${r.requested} kg
    ├─ Allocated : ${r.allocated} kg
    └─ Status    : ${r.status}`;
    }
  } else {
    txt += '  No allocation run yet.';
  }

  txt += `

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 ALGORITHM COMPLEXITY
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  Priority Calculation (C++)  : Time O(n)        | Space O(1)
  Sorting Algorithm   (C++)   : Time O(n log n)   | Space O(n)
  Greedy Allocation   (C++)   : Time O(n)         | Space O(1)
  Frontend Algorithms  (JS)   : Time O(n log n)   | Space O(n)
  ─────────────────────────────────────────────────────────────
  OVERALL                     : Time O(n log n)   | Space O(n)

╔══════════════════════════════════════════════════════════════╗
║                        END OF REPORT                         ║
╚══════════════════════════════════════════════════════════════╝`;

  const el = document.getElementById('report-content');
  if (el) el.innerHTML = `<pre style="white-space:pre;font-family:var(--font-mono);font-size:13px;line-height:1.7">${txt}</pre>`;
  showToast('📋 Report generated', 'success');
}

function printReport() {
  window.print();
}

function updateLiveComplexity(data, algoIdx) {
  const el = document.getElementById('live-complexity');
  const pre = document.getElementById('live-complexity-content');
  if (!el || !pre) return;
  el.style.display = 'block';
  pre.textContent = `Algorithm Used : ${getAlgorithmName(algoIdx)}
Time Complexity: ${getTimeComplexity(algoIdx)}
Space Complexity: ${getSpaceComplexity(algoIdx)}

Greedy Allocation:
  Time : O(n)
  Space: O(1)
  NGOs processed: ${data.results?.length || 0}
  Total Allocated: ${data.totalAllocated || 0} kg
  Efficiency: ${(data.efficiency || 0).toFixed(1)}%`;
}

function getAlgorithmName(i) {
  return ['QuickSort','MergeSort','HeapSort','BubbleSort','SelectionSort','InsertionSort'][i] || 'Unknown';
}
function getTimeComplexity(i) {
  return ['O(n log n) avg','O(n log n)','O(n log n)','O(n²)','O(n²)','O(n²)'][i] || '—';
}
function getSpaceComplexity(i) {
  return ['O(log n)','O(n)','O(1)','O(1)','O(1)','O(1)'][i] || '—';
}

// ── MODAL HELPERS ───────────────────────────────────────────
function showAddNGOForm() {
  document.getElementById('modal-add-ngo').style.display = 'flex';
}
function hideAddNGOForm() {
  document.getElementById('modal-add-ngo').style.display = 'none';
  ['ngo-name','ngo-urgency','ngo-beneficiaries','ngo-quantity'].forEach((id, i) => {
    const el = document.getElementById(id);
    if (el) el.value = ['','5','100','50'][i];
  });
}
function showAddFoodForm() {
  document.getElementById('modal-add-food').style.display = 'flex';
}
function hideAddFoodForm() {
  document.getElementById('modal-add-food').style.display = 'none';
  ['supplier-name','food-type','food-quantity'].forEach((id,i) => {
    const el = document.getElementById(id);
    if (el) el.value = ['','','50'][i];
  });
}

document.addEventListener('click', e => {
  if (e.target.classList.contains('modal-overlay')) {
    e.target.style.display = 'none';
  }
});

function showToast(msg, type = 'info') {
  const container = document.getElementById('toast-container');
  if (!container) return;
  const toast = document.createElement('div');
  const icons = { success: '✅', error: '❌', warning: '⚠️', info: 'ℹ️' };
  toast.className = `toast toast-${type}`;
  toast.innerHTML = `<span>${icons[type]||'ℹ️'}</span><span>${msg}</span>`;
  container.appendChild(toast);
  setTimeout(() => { toast.style.opacity = '0'; toast.style.transform = 'translateX(40px)'; toast.style.transition = 'all .3s'; setTimeout(() => toast.remove(), 300); }, 3500);
}

function setText(id, val) {
  const el = document.getElementById(id);
  if (el) el.textContent = val;
}

// ==================== MAP INTEGRATION - DEHRADUN ====================

function initFoodMap() {
    if (map) return;
    
    map = L.map('food-map').setView([30.3165, 78.0322], 13);
    
    L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a>',
        subdomains: 'abcd',
        maxZoom: 19,
        minZoom: 10
    }).addTo(map);
    
    console.log('Dehradun Map initialized');
}

function updateMapMarkers() {
    if (!map) initFoodMap();
    
    mapMarkers.forEach(marker => map.removeLayer(marker));
    mapMarkers = [];
    
    if (!ngos || ngos.length === 0) return;
    
    ngos.forEach((ngo) => {
        const isNewlyAdded = newlyAddedNGOIds.includes(ngo.id);
        
        let lat = ngo.latitude || (30.3165 + (ngo.id * 0.01));
        let lng = ngo.longitude || (78.0322 + (ngo.id * 0.008));
        
        let markerColor;
        let markerSize = 14;
        let borderWidth = 2;
        let glowEffect = '';
        let priorityLabel = '';
        
        if (isNewlyAdded) {
            markerColor = '#9b59b6';
            markerSize = 18;
            borderWidth = 3;
            glowEffect = 'box-shadow: 0 0 0 3px rgba(155, 89, 182, 0.5), 0 0 0 6px rgba(155, 89, 182, 0.3);';
        } else if (ngo.priorityScore >= 800) {
            markerColor = '#e74c3c';
            priorityLabel = '🔴 High Priority';
        } else if (ngo.priorityScore >= 500) {
            markerColor = '#f5a623';
            priorityLabel = '🟠 Medium Priority';
        } else {
            markerColor = '#2ecc71';
            priorityLabel = '🟢 Low Priority';
        }
        
        const marker = L.marker([lat, lng], {
            icon: L.divIcon({
                className: 'custom-marker',
                html: `<div class="marker-dot ${isNewlyAdded ? 'new-marker' : ''}" 
                            style="background-color: ${markerColor}; 
                                   width: ${markerSize}px; 
                                   height: ${markerSize}px; 
                                   border-radius: 50%; 
                                   border: ${borderWidth}px solid white; 
                                   ${glowEffect}
                                   transition: all 0.2s ease;
                                   cursor: pointer;
                                   ${isNewlyAdded ? 'animation: pulse 1.5s infinite;' : ''}"
                            onmouseover="this.style.transform='scale(1.4)'; this.style.boxShadow='0 0 0 4px rgba(0,0,0,0.2)'"
                            onmouseout="this.style.transform='scale(1)'; this.style.boxShadow='${isNewlyAdded ? '0 0 0 3px rgba(155,89,182,0.5)' : 'none'}'">
                         </div>`,
                iconSize: [markerSize + 8, markerSize + 8],
                popupAnchor: [0, -12]
            })
        }).addTo(map);
        
        const isNewBadge = isNewlyAdded ? '<span style="background: #9b59b6; color: white; padding: 2px 8px; border-radius: 12px; font-size: 10px; margin-left: 8px;">✨ NEW</span>' : '';
        
        const popupContent = `
            <div style="font-family: 'DM Sans', sans-serif; min-width: 220px; max-width: 280px;">
                <div style="display: flex; align-items: center; gap: 8px; margin-bottom: 8px; border-bottom: 2px solid ${markerColor}; padding-bottom: 6px;">
                    <span style="font-size: 20px;">🏢</span>
                    <b style="font-size: 14px; color: #2c3e50;">${ngo.name}</b>
                    ${isNewBadge}
                </div>
                <div style="margin: 8px 0;">
                    ${!isNewlyAdded ? `
                    <div style="display: flex; justify-content: space-between; margin: 4px 0;">
                        <span style="color: #666;">⭐ Priority:</span>
                        <span style="font-weight: bold; color: ${markerColor};">${ngo.priorityScore || 'N/A'}</span>
                    </div>` : `
                    <div style="display: flex; justify-content: space-between; margin: 4px 0;">
                        <span style="color: #666;">✨ Status:</span>
                        <span style="font-weight: bold; color: #9b59b6;">Newly Registered</span>
                    </div>`}
                    <div style="display: flex; justify-content: space-between; margin: 4px 0;">
                        <span style="color: #666;">⚠️ Urgency:</span>
                        <span style="font-weight: bold;">${ngo.urgency}/10</span>
                    </div>
                    <div style="display: flex; justify-content: space-between; margin: 4px 0;">
                        <span style="color: #666;">👥 Beneficiaries:</span>
                        <span style="font-weight: bold;">${ngo.beneficiaries?.toLocaleString() || 0}</span>
                    </div>
                    <div style="display: flex; justify-content: space-between; margin: 4px 0;">
                        <span style="color: #666;">🍱 Requested:</span>
                        <span style="font-weight: bold;">${ngo.quantity || 0} kg</span>
                    </div>
                </div>
                <hr style="margin: 6px 0;">
                <div style="display: flex; gap: 8px; margin-top: 8px;">
                    <button onclick="zoomToLocation(${lat}, ${lng}, '${ngo.name}')" 
                            style="background: #4f8ef7; color: white; border: none; padding: 5px 12px; border-radius: 6px; cursor: pointer; font-size: 11px;">
                        🔍 Zoom Here
                    </button>
                    <button onclick="deleteNGO(${ngo.id})" 
                            style="background: #e74c3c; color: white; border: none; padding: 5px 12px; border-radius: 6px; cursor: pointer; font-size: 11px;">
                        🗑 Delete
                    </button>
                </div>
            </div>
        `;
        
        marker.bindPopup(popupContent);
        marker.bindTooltip(`${ngo.name}${isNewlyAdded ? ' ✨ NEW' : ''}`, { permanent: false, direction: 'top', offset: [0, -10] });
        marker.on('click', function() { map.setView([lat, lng], 16); this.openPopup(); });
        
        mapMarkers.push(marker);
    });
    
    if (mapMarkers.length > 0) {
        const group = L.featureGroup(mapMarkers);
        map.fitBounds(group.getBounds().pad(0.1));
    }
}

function zoomToLocation(lat, lng, name) {
    if (map) {
        map.setView([lat, lng], 17);
        showToast(`📍 Zoomed to ${name}`, 'success');
    }
}

function centerMapDehradun() {
    if (map) {
        map.setView([30.3165, 78.0322], 13);
        showToast('📍 Centered to Dehradun', 'info');
    }
}

function addUserLocation() {
    if (!map) initFoodMap();
    
    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(
            (position) => {
                currentUserLocation = {
                    lat: position.coords.latitude,
                    lng: position.coords.longitude
                };
                
                const userMarker = L.marker([currentUserLocation.lat, currentUserLocation.lng], {
                    icon: L.divIcon({
                        className: 'user-marker',
                        html: `<div style="background-color: #4f8ef7; width: 18px; height: 18px; border-radius: 50%; border: 2px solid white; box-shadow: 0 0 0 2px #4f8ef7; animation: pulse 1.5s infinite;"></div>`,
                        iconSize: [24, 24],
                        popupAnchor: [0, -12]
                    })
                }).addTo(map);
                userMarker.bindPopup('<b>📍 You are here</b><br>Your current location').openPopup();
                map.setView([currentUserLocation.lat, currentUserLocation.lng], 14);
                showToast('📍 Your location added!', 'success');
            },
            (error) => console.log('Geolocation error:', error)
        );
    }
}

function refreshMap() {
    updateMapMarkers();
    showToast('🗺️ Map refreshed!', 'info');
}

function searchAndZoomNGO() {
    const searchTerm = prompt("Enter NGO name to search:\n\n" + validNGOLocations.map(l => l.name).join("\n"));
    if (searchTerm) {
        const location = getNGOLocation(searchTerm);
        if (location) {
            zoomToLocation(location.lat, location.lng, searchTerm);
        } else {
            showToast(`❌ "${searchTerm}" not found in Dehradun NGOs list`, 'error');
        }
    }
}