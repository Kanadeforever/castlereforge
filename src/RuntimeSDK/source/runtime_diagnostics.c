#include "runtime_internal.h"

static volatile LONG g_diagnostic_lock;
static char g_diagnostic_text[RUNTIME_DIAGNOSTIC_CAP];
static CastleU32 g_diagnostic_length;
static volatile LONG g_diagnostic_generation;

void Runtime_DiagnosticsInitialize(void) {
    g_diagnostic_lock = 0;
    g_diagnostic_length = 0u;
    g_diagnostic_text[0] = '\0';
    g_diagnostic_generation = 0;
}

void Runtime_DiagnosticAppend(const char* utf8_line) {
    CastleU32 line_length;
    CastleU32 index;

    if (!utf8_line) return;
    line_length = Runtime_StringLength(utf8_line);
    if (line_length == 0u || line_length + 3u > RUNTIME_DIAGNOSTIC_CAP) return;

    Runtime_Lock(&g_diagnostic_lock);

    /*
     * 第一版使用固定线性缓冲区。写满后从头开始新一代记录，避免堆分配和复杂环形切行。
     * generation 仍持续增加，因此工具能知道旧文字已经被覆盖。
     */
    if (g_diagnostic_length + line_length + 3u > RUNTIME_DIAGNOSTIC_CAP) {
        g_diagnostic_length = 0u;
        g_diagnostic_text[0] = '\0';
    }

    for (index = 0u; index < line_length; ++index) {
        g_diagnostic_text[g_diagnostic_length++] = utf8_line[index];
    }
    g_diagnostic_text[g_diagnostic_length++] = '\r';
    g_diagnostic_text[g_diagnostic_length++] = '\n';
    g_diagnostic_text[g_diagnostic_length] = '\0';
    InterlockedIncrement(&g_diagnostic_generation);

    Runtime_Unlock(&g_diagnostic_lock);
}

CastleU32 Runtime_DiagnosticGeneration(void) {
    return (CastleU32)InterlockedCompareExchange(&g_diagnostic_generation, 0, 0);
}

CastleResult Runtime_CopyDiagnostics(CastleU32 after_generation,
                                     CastleDiagnosticBufferV1* output) {
    CastleU32 current_generation;

    if (!output || output->magic != CASTLE_DIAGNOSTIC_BUFFER_MAGIC ||
        output->struct_size < CASTLE_SIZEOF_DIAGNOSTIC_BUFFER_V1 ||
        output->buffer_version != CASTLE_DIAGNOSTIC_BUFFER_VERSION_1) {
        return CASTLE_ERROR_INVALID_ARGUMENT;
    }

    Runtime_Lock(&g_diagnostic_lock);
    current_generation = (CastleU32)InterlockedCompareExchange(
        &g_diagnostic_generation, 0, 0);

    output->bytes_written = 0u;
    output->first_generation = current_generation ? 1u : 0u;
    output->last_generation = current_generation;
    output->required_capacity = 1u;

    if (after_generation >= current_generation) {
        if (!output->utf8_buffer || output->buffer_capacity == 0u) {
            Runtime_Unlock(&g_diagnostic_lock);
            return CASTLE_ERROR_BUFFER_TOO_SMALL;
        }
        output->utf8_buffer[0] = '\0';
        Runtime_Unlock(&g_diagnostic_lock);
        return CASTLE_OK;
    }

    output->required_capacity = g_diagnostic_length + 1u;
    if (!output->utf8_buffer || output->buffer_capacity < output->required_capacity) {
        Runtime_Unlock(&g_diagnostic_lock);
        return CASTLE_ERROR_BUFFER_TOO_SMALL;
    }

    Runtime_ByteCopy(output->utf8_buffer, g_diagnostic_text, g_diagnostic_length + 1u);
    output->bytes_written = g_diagnostic_length;
    Runtime_Unlock(&g_diagnostic_lock);
    return CASTLE_OK;
}
