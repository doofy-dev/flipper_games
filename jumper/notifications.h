#pragma once
#include <notification/notification.h>
#include <notification/notification_messages.h>

const NotificationSequence sequence_fail = {
        &message_vibro_on,
        &message_note_c4,
        &message_delay_10,
        &message_vibro_off,
        &message_sound_off,
        &message_delay_10,

        &message_vibro_on,
        &message_note_a3,
        &message_delay_10,
        &message_vibro_off,
        &message_sound_off,
        NULL,
};
