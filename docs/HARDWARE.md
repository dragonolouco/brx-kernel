# Matriz de hardware do BRX

O BRX não deve depender de um único computador. A estratégia é separar barramento, classe de dispositivo e driver concreto. Assim, um adaptador Wi-Fi desconhecido pode continuar sendo identificado, enquanto um driver compatível é carregado quando existir.

| Família | Detecção | Primeiro suporte BRX | Próxima expansão |
|---|---|---|---|
| Teclado PS/2 | controlador legado e IRQ1 | Driver PS/2 inicial | scancodes para eventos BRX e terminal |
| Mouse PS/2 | segundo canal e IRQ12 | Inicialização e captura de bytes | protocolo de três/ quatro bytes e eventos de ponteiro |
| Teclado/mouse USB | PCI + xHCI/EHCI/OHCI/UHCI | planejado | USB HID e hotplug |
| Vídeo | PCI class 03 | inventariado | framebuffer, VBE/EFI GOP, virtio-gpu e DRM BRX |
| Armazenamento | PCI class 01 | inventariado | virtio-blk, AHCI/SATA, NVMe e USB mass storage |
| Ethernet | PCI class 02 | inventariado | virtio-net, e1000/e1000e, Realtek e USB Ethernet |
| Wi-Fi PCIe | PCI class 02 e IDs do fabricante | inventariado | drivers por família, firmware separado e WPA supplicant BRX |
| Wi-Fi USB | USB class wireless/vendor ID | planejado | RTL8xxx, MT76, ath9k-htc e adaptadores genéricos suportados |
| Bluetooth | USB class wireless | planejado | HCI USB, emparelhamento e serviço Bluetooth BRX |
| Áudio | PCI class 04 | inventariado | Intel HDA, AC97, USB Audio e mixer |
| RTC/relógio | ACPI/CMOS/HPET | planejado | relógio monotônico, wall clock e timers de alta resolução |
| Energia | ACPI tables | planejado | bateria, suspensão, desligamento, thermal zones |
| Impressoras/scanners | USB class e rede | planejado | transporte USB/IP e camada de dispositivos |
| Câmeras | USB Video Class | planejado | captura V4L-like na API BRX |

## Hardware sem suporte específico

O núcleo deve diferenciar quatro situações: dispositivo ausente, dispositivo detectado mas sem driver, driver presente sem firmware e driver operacional. A enumeração PCI/USB continua disponível em todos os casos. Isso permite que a camada de user space instale firmware, carregue um módulo ou indique claramente que o adaptador ainda não é suportado.

Não existe um “driver Wi-Fi universal” seguro. Cada chipset possui registros, DMA, firmware, calibração e comportamento próprios. Para adaptadores sem Wi-Fi integrado, o caminho é o mesmo: USB ou PCIe, enumeração do dispositivo, identificação do fabricante/modelo, módulo específico e serviço de rede BRX.

## Ordem de implementação

A ordem prática é: barramentos PCI/USB e DMA; console e input; armazenamento; rede cabeada; Wi-Fi e Bluetooth; áudio/vídeo; energia e hotplug. Drivers de hardware ficam fora da camada de compatibilidade Linux, mas podem expor uma API BRX comum para POSIX, Linux ABI e ferramentas de user space.
