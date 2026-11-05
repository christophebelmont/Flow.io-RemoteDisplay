# Flow.io Remote Display

Firmware PlatformIO dédié à `FlowConnectDisplay`. Il s'exécute sur un ESP32
standard (ESP32 DevKit V1, sans PSRAM) et pilote un écran Nextion local par UART.
La communication avec le contrôleur Flow.io/Waveshare se fait en UDP sur le
réseau Wi-Fi.

## Périmètre du firmware

Le profil `FlowConnectDisplay` est volontairement réduit aux fonctions
nécessaires à l'afficheur distant :

- connexion Wi-Fi et portail de configuration/provisionnement ;
- logs série et configuration persistée ;
- pilote Nextion et client UDP Flow Connect Display ;
- protocole binaire partagé avec Flow.io sur le port UDP `42110`.

Le firmware ne contient pas de client MQTT ni de configuration broker. Il
n'active pas PSRAM et n'utilise pas le serveur HTTP asynchrone. Le portail
utilise les composants réseau inclus dans le framework Arduino ESP32. Ses
ressources web compressées sont embarquées dans le firmware; le portail AP ne
monte pas SPIFFS. Le dépôt
conserve les sources communes Flow.io pour aligner le profil et les interfaces
de modules sur le firmware principal ; le filtre PlatformIO du profil exclut
les fonctions qui ne servent pas à l'afficheur.

## Compilation et téléversement

PlatformIO sélectionne `FlowConnectDisplay` par défaut :

```sh
pio run
pio run -t upload
pio device monitor
```

La cible est définie dans `platformio.ini` avec `board = esp32doit-devkit-v1`.
Le terminal série est configuré à `115200` bauds.

## Mise en service

1. Flasher le firmware sur l'ESP32 et ouvrir le moniteur série.
2. Configurer le Wi-Fi depuis le portail de provisionnement si aucun réseau
   enregistré n'est disponible.
3. Raccorder le Nextion en UART à `115200` bauds : TX Nextion vers GPIO16/RX
   ESP32, RX Nextion depuis GPIO17/TX ESP32, avec une masse commune.
4. Mettre le contrôleur Flow.io/Waveshare sur le même réseau. Le client distant
   diffuse `Hello`, puis établit le lien avec le contrôleur sur UDP/42110.
5. Laisser le token UDP vide des deux côtés pour l'appairage par défaut. Si un
   token est activé côté Flow.io, FlowConnectDisplay doit recevoir le même
   token dans `fcd/udp/token` via sa configuration persistante.

Le portail captif configure le Wi-Fi et permet les mises à jour ESP32 et
Nextion. Il ne propose pas encore de champ pour le token UDP. La configuration
par défaut, avec token vide, permet l'appairage sans étape supplémentaire.

En mode point d'accès, la page permet aussi de téléverser un firmware ESP32
`.bin`. L'image est écrite par blocs dans le slot OTA inactif et vérifiée avant
le redémarrage. La table de partitions comprend deux slots OTA de 1,5 Mio.
Les appareils encore dotés de l'ancienne table à slot unique doivent recevoir
une première installation complète par USB/série avant d'accepter une mise à
jour OTA depuis la page web.

## Contrat UDP

Le format des paquets et leurs contrôles sont définis dans
`src/Core/Hmi/HmiUdpProtocol.h`. Le client distant gère découverte, `Welcome`,
keepalive, ACK/retransmission, rendu de l'écran d'accueil et du menu de
configuration, événements tactiles et synchronisation RTC. Les messages sont
binaires compacts et ne transportent pas de JSON.

La documentation d'intégration du firmware Flow.io est disponible dans
`docs/remote-display-udp.md`.
