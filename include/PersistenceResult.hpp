#pragma once

/** @brief Outcome of a sequential save or weekly log operation. */
enum class PersistenceResult {
    Ok,
    NotFound,
    InvalidData,
    IoError
};
