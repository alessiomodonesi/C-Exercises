// keyboard_lock.c
// Compila: clang keyboard_lock.c -o keyboard_lock -framework ApplicationServices -framework CoreFoundation

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

static CFMachPortRef g_tap = NULL;

static CGEventRef event_callback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *userInfo)
{
    (void)proxy;
    (void)userInfo;

    // Se il tap viene disabilitato (timeout/utente), riabilitalo
    if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput)
    {
        fprintf(stderr, "[i] Event tap disabilitato -> riabilito.\n");
        if (g_tap)
            CGEventTapEnable(g_tap, true);
        return event;
    }

    // Scarta TUTTI gli eventi tastiera
    if (type == kCGEventKeyDown || type == kCGEventKeyUp || type == kCGEventFlagsChanged)
    {
        return NULL;
    }

    // Per qualsiasi altro evento (non dovrebbe capitare con la mask), lascia passare
    return event;
}

int main(void)
{
    fprintf(stderr, "[i] Tastiera BLOCCATA. Nessuna hotkey di sblocco.\n");
    fprintf(stderr, "[i] Per uscire termina il processo (Ctrl+C se passa, oppure `kill` dal terminale/Monitoraggio Attività).\n");
    fprintf(stderr, "[i] Concedi i permessi in Impostazioni → Privacy e sicurezza → Accessibilità/Monitoraggio input.\n");

    CGEventMask mask =
        CGEventMaskBit(kCGEventKeyDown) |
        CGEventMaskBit(kCGEventKeyUp) |
        CGEventMaskBit(kCGEventFlagsChanged);

    g_tap = CGEventTapCreate(
        kCGSessionEventTap,       // eventi della sessione utente
        kCGHeadInsertEventTap,    // intercetta il prima possibile
        kCGEventTapOptionDefault, // attivo
        mask,
        event_callback,
        NULL);

    if (!g_tap)
    {
        fprintf(stderr, "[!] Impossibile creare l'Event Tap. Controlla i permessi.\n");
        return 1;
    }

    CFRunLoopSourceRef src = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, g_tap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), src, kCFRunLoopCommonModes);
    CGEventTapEnable(g_tap, true);

    CFRunLoopRun(); // blocco attivo

    // Pulizia (in pratica ci arrivi solo se fermato con segnale che permette cleanup)
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), src, kCFRunLoopCommonModes);
    CFRelease(src);
    CFRelease(g_tap);
    g_tap = NULL;

    fprintf(stderr, "[i] Uscita. Tastiera sbloccata.\n");
    return 0;
}
