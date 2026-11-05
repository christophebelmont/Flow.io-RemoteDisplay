const $ = (id) => document.getElementById(id);

const ui = {
  product: "flow.io",
  rebootAfterWifi: false,
  isDisplay: false
};

const webSerial = {
  cursor: 0,
  session: "",
  paused: false,
  history: "",
  timer: 0,
  started: false
};

const moduleLogLevels = {
  modules: [],
  loading: false
};

function status(text) {
  $("status").textContent = text;
}

async function json(url, options) {
  const res = await fetch(url, options);
  const data = await res.json().catch(() => null);
  if (!res.ok || !data || data.ok === false) throw new Error(url);
  return data;
}

function form(values) {
  return new URLSearchParams(values);
}

function applyBrand(meta) {
  const profile = String((meta && (meta.profile || meta.profile_name)) || "").toLowerCase();
  const product = String((meta && meta.product_name) || "");
  const isDisplay = product === "Flow Connect Display" || profile.indexOf("flowconnectdisplay") >= 0 || profile.indexOf("flow_connect_display") >= 0;
  const name = isDisplay ? "Flow Connect Display" : profile === "micronova" ? "Pellet" : "Flow";
  const suffix = document.querySelector(".brand-io");
  $("brandName").textContent = name;
  if (suffix) suffix.textContent = isDisplay ? "" : ".io";
  $("brand").setAttribute("aria-label", isDisplay ? name : name + ".io");
  document.title = isDisplay ? name : name + ".io";
}

function applyCapabilities(meta) {
  ui.product = String((meta && meta.product_name) || ui.product);
  ui.rebootAfterWifi = !!(meta && meta.reboot_after_wifi_save);
  const profile = String((meta && (meta.profile || meta.profile_name)) || "").toLowerCase();
  ui.isDisplay = ui.product === "Flow Connect Display" ||
    profile.indexOf("flowconnectdisplay") >= 0 ||
    profile.indexOf("flow_connect_display") >= 0;
  $("firmwareBox").classList.toggle("hidden", !ui.isDisplay);
  $("nextionFirmwareBox").classList.toggle("hidden", !ui.isDisplay);
  $("webSerialBox").classList.toggle("hidden", !ui.isDisplay);
  if (ui.isDisplay && !webSerial.started) {
    webSerial.started = true;
    pollWebSerial();
  }
}

function setWebSerialStatus(message) {
  $("webSerialStatus").textContent = message;
}

function appendWebSerialText(text) {
  if (!text) return;
  webSerial.history = (webSerial.history + text).slice(-65536);
  const output = $("webSerialOutput");
  output.textContent = webSerial.history;
  if ($("webSerialFollow").checked) output.scrollTop = output.scrollHeight;
}

async function pollWebSerial() {
  if (!webSerial.paused && !document.hidden) {
    const controller = new AbortController();
    const timeout = setTimeout(() => controller.abort(), 4000);
    try {
      const query = new URLSearchParams({
        cursor: String(webSerial.cursor),
        session: webSerial.session
      });
      const response = await fetch("/api/logs?" + query.toString(), {
        cache: "no-store",
        signal: controller.signal
      });
      if (!response.ok) throw new Error("HTTP " + response.status);
      const text = await response.text();
      const nextSession = response.headers.get("X-Log-Session") || "";
      if (webSerial.session && webSerial.session !== nextSession) {
        appendWebSerialText("\n— ESP redémarré —\n");
      }
      webSerial.session = nextSession;
      webSerial.cursor = Number(response.headers.get("X-Log-Cursor")) || 0;
      const lost = Number(response.headers.get("X-Log-Lost")) || 0;
      if (lost) appendWebSerialText("— " + lost + " anciennes lignes non conservées —\n");
      appendWebSerialText(text);
      setWebSerialStatus("Connecté · actualisation chaque seconde");
    } catch (err) {
      setWebSerialStatus("Connexion interrompue · nouvelle tentative automatique");
    } finally {
      clearTimeout(timeout);
    }
  }
  webSerial.timer = setTimeout(pollWebSerial, 1000);
}

function initializeWebSerial() {
  refreshModuleLogLevels();
  $("webSerialPause").addEventListener("click", () => {
    webSerial.paused = !webSerial.paused;
    $("webSerialPause").textContent = webSerial.paused ? "Reprendre" : "Pause";
    setWebSerialStatus(webSerial.paused ? "En pause" : "Reprise…");
  });
  $("webSerialClear").addEventListener("click", () => {
    webSerial.history = "";
    $("webSerialOutput").textContent = "";
  });
  $("webSerialSave").addEventListener("click", () => {
    const url = URL.createObjectURL(new Blob([webSerial.history], { type: "text/plain;charset=utf-8" }));
    const link = document.createElement("a");
    link.href = url;
    link.download = "flowio-logs.txt";
    link.click();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
  });
}

function renderModuleLogLevels() {
  const container = $("moduleLogLevels");
  container.textContent = "";
  for (const module of moduleLogLevels.modules) {
    const row = document.createElement("div");
    row.className = "module-log-row";

    const description = document.createElement("div");
    const name = document.createElement("div");
    name.className = "module-log-name";
    name.textContent = module.name;
    const current = document.createElement("div");
    current.className = "module-log-state";
    const levelNames = { debug: "Debug", info: "Info", warn: "Warning", error: "Error" };
    current.textContent = "Actif : " + (levelNames[module.level] || "Info");
    description.append(name, current);

    const actions = document.createElement("div");
    actions.className = "module-log-actions";
    for (const level of ["debug", "info"]) {
      const button = document.createElement("button");
      button.type = "button";
      button.className = "secondary" + (module.level === level ? " selected" : "");
      button.textContent = level === "debug" ? "Debug" : "Info";
      button.disabled = moduleLogLevels.loading;
      button.addEventListener("click", () => setModuleLogLevel(module.id, level));
      actions.appendChild(button);
    }
    row.append(description, actions);
    container.appendChild(row);
  }
}

async function refreshModuleLogLevels() {
  try {
    const data = await json("/api/log-levels", { cache: "no-store" });
    moduleLogLevels.modules = Array.isArray(data.modules) ? data.modules : [];
    renderModuleLogLevels();
  } catch (err) {
    $("moduleLogLevels").textContent = "Réglage des niveaux de logs indisponible.";
  }
}

async function setModuleLogLevel(moduleId, level) {
  moduleLogLevels.loading = true;
  renderModuleLogLevels();
  try {
    await json("/api/log-levels", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: form({ module: String(moduleId), level })
    });
    await refreshModuleLogLevels();
  } catch (err) {
    setWebSerialStatus("Impossible de modifier le niveau de log.");
  } finally {
    moduleLogLevels.loading = false;
    renderModuleLogLevels();
  }
}

let scanTimer = 0;

function scanStatus(text) {
  $("wifiScanStatus").textContent = text;
}

function renderWifiScan(data) {
  const nets = data && Array.isArray(data.networks) ? data.networks : [];
  const list = $("wifiSsidList");
  const current = ($("wifiSsid").value || "").trim();
  list.innerHTML = "";
  const manual = document.createElement("option");
  manual.value = "";
  manual.textContent = "Saisie manuelle";
  list.appendChild(manual);
  for (const net of nets) {
    if (!net || !net.ssid || net.hidden) continue;
    const opt = document.createElement("option");
    const secure = net.secure ? " (securise)" : " (ouvert)";
    const rssi = Number.isFinite(net.rssi) ? " " + net.rssi + " dBm" : "";
    opt.value = net.ssid;
    opt.textContent = (net.ssid + secure + rssi).slice(0, 72);
    list.appendChild(opt);
  }
  if (current) {
    for (const opt of list.options) {
      if (opt.value === current) {
        list.value = current;
        break;
      }
    }
  }
}

function updateScanStatus(data) {
  if (!data || data.ok !== true) {
    scanStatus("Scan réseau indisponible.");
    return;
  }
  const running = !!data.running || !!data.requested;
  const count = Number.isFinite(data.count) ? data.count : 0;
  const total = Number.isFinite(data.total_found) ? data.total_found : count;
  if (running) {
    scanStatus("Scan réseau en cours...");
    return;
  }
  scanStatus(count > 0
    ? "Scan réseau terminé : " + count + " réseaux affichés (" + total + " détectés)."
    : "Aucun réseau visible détecté.");
}

async function refreshWifiScan(trigger) {
  clearTimeout(scanTimer);
  try {
    if (trigger) {
      await json("/api/wifi/scan", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: form({ force: "1" })
      });
      scanStatus("Scan réseau lancé...");
    }

    let data = null;
    let lastErr = null;
    for (let attempt = 0; attempt < 8; attempt += 1) {
      try {
        data = await json("/api/wifi/scan", { cache: "no-store" });
        renderWifiScan(data);
        updateScanStatus(data);
        if (!data.running && !data.requested) break;
      } catch (err) {
        lastErr = err;
      }
      await new Promise((resolve) => setTimeout(resolve, 450));
    }

    if (!data && lastErr) {
      throw lastErr;
    }
    if (data && (data.running || data.requested)) {
      scanTimer = setTimeout(() => refreshWifiScan(false), 1200);
    }
  } catch (err) {
    scanStatus("Erreur scan réseau: " + err.message);
  }
}

async function loadAll() {
  try {
    const meta = await json("/api/web/meta");
    applyBrand(meta);
    applyCapabilities(meta);
    $("subtitle").textContent = "Configuration Initiale - " + (meta.firmware_version || "flow.io");
  } catch (err) {
    status("Meta indisponible");
  }

  try {
    const wifi = await json("/api/wifi/config");
    $("wifiEnabled").checked = !!wifi.enabled;
    $("wifiSsid").value = wifi.ssid || "";
    $("wifiPass").value = wifi.pass || "";
    status("Pret");
  } catch (err) {
    status("Erreur réseau: " + err.message);
  }
}

async function saveWifi() {
  try {
    const data = await json("/api/wifi/config", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: form({
        enabled: $("wifiEnabled").checked ? "1" : "0",
        ssid: $("wifiSsid").value,
        pass: $("wifiPass").value
      })
    });
    status((ui.rebootAfterWifi || data.reboot_scheduled)
      ? "Réseau enregistre. Redemarrage en cours..."
      : "Réseau enregistre. Redemarrage immediat...");
    await fetch("/api/system/reboot", { method: "POST" });
  } catch (err) {
    status("Erreur réseau: " + err.message);
  }
}

function uploadFirmwareFile(options) {
  const file = $(options.fileId).files[0];
  const statusNode = $(options.statusId);
  if (!file) {
    statusNode.textContent = "Sélectionnez un fichier " + options.extension + ".";
    return;
  }
  if (!file.name.toLowerCase().endsWith(options.extension)) {
    statusNode.textContent = "Le fichier doit avoir l’extension " + options.extension + ".";
    return;
  }
  if (!window.confirm(options.confirmText)) return;

  const button = $(options.buttonId);
  const progress = $(options.progressId);
  const request = new XMLHttpRequest();
  button.disabled = true;
  progress.value = 0;
  statusNode.textContent = "Envoi du firmware...";
  request.open("POST", options.endpoint);
  request.setRequestHeader("Content-Type", "application/octet-stream");
  request.timeout = 180000;
  request.upload.onprogress = (event) => {
    if (!event.lengthComputable) return;
    progress.value = Math.round((event.loaded / event.total) * 100);
    statusNode.textContent = "Envoi du firmware : " + progress.value + "%";
  };
  request.onload = () => {
    let result = null;
    try { result = JSON.parse(request.responseText); } catch (err) { /* handled below */ }
    if (request.status === 202 && result && result.ok) {
      progress.value = 100;
      statusNode.textContent = options.successText;
      return;
    }
    statusNode.textContent = options.failureText ||
      "Mise à jour refusée ou invalide (HTTP " + request.status + ").";
    button.disabled = false;
  };
  request.onerror = () => {
    statusNode.textContent = "Connexion interrompue. Vérifiez l’état de l’équipement avant de réessayer.";
    button.disabled = false;
  };
  request.ontimeout = () => {
    statusNode.textContent = "Délai d’envoi dépassé. Vérifiez l’état de l’équipement avant de réessayer.";
    button.disabled = false;
  };
  request.send(file);
}

function uploadEspFirmware() {
  uploadFirmwareFile({
    fileId: "firmwareFile",
    buttonId: "firmwareUpload",
    progressId: "firmwareProgress",
    statusId: "firmwareStatus",
    extension: ".bin",
    endpoint: "/api/system/firmware",
    confirmText: "L’ESP va redémarrer après la mise à jour. Continuer ?",
    successText: "Firmware reçu et vérifié. Redémarrage de l’ESP..."
  });
}

function uploadNextionFirmware() {
  uploadFirmwareFile({
    fileId: "nextionFirmwareFile",
    buttonId: "nextionFirmwareUpload",
    progressId: "nextionFirmwareProgress",
    statusId: "nextionFirmwareStatus",
    extension: ".tft",
    endpoint: "/api/system/nextion-firmware",
    confirmText: "L’écran Nextion va redémarrer après la mise à jour. Continuer ?",
    successText: "Firmware reçu et acquitté. Redémarrage du Nextion...",
    failureText: "Le transfert a échoué. Le Nextion peut nécessiter un redémarrage ou un nouveau chargement."
  });
}

$("wifiScanBtn").addEventListener("click", () => refreshWifiScan(true));
$("wifiSsidList").addEventListener("change", () => {
  const value = $("wifiSsidList").value;
  if (value) $("wifiSsid").value = value;
});
$("wifiSave").addEventListener("click", () => saveWifi());
$("firmwareUpload").addEventListener("click", () => uploadEspFirmware());
$("nextionFirmwareUpload").addEventListener("click", () => uploadNextionFirmware());
initializeWebSerial();
loadAll();
