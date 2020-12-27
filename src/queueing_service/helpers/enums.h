#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_ENUMS_H
#define QUEUEING_SERVICE_HELPERS_ENUMS_H

namespace queueing_service {
	enum class receiver : short {
		client,
		service
	};
}

#endif