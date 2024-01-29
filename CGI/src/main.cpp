/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankinzin <ankinzin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 14:37:39 by ankinzin          #+#    #+#             */
/*   Updated: 2024/01/25 14:40:36 by ankinzin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

int main() {
	// Set the Content-Type header
	CGIExample::printHeader();

	// Retrieve request method
	const char* requestMethod = std::getenv("REQUEST_METHOD");

	// Process form data based on the request method
	if (requestMethod && std::string(requestMethod) == "POST") {
		CGIExample::processFormData();
	} else {
		CGIExample::printForm();
	}

	return 0;
}
