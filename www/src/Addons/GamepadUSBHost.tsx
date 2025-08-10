import React, { useContext } from 'react';
import { useTranslation } from 'react-i18next';
import { FormCheck, FormLabel, Row } from 'react-bootstrap';
import * as yup from 'yup';

import Section from '../Components/Section';

import FormControl from '../Components/FormControl';
import { AppContext } from '../Contexts/AppContext';

export const gamepadUSBHostScheme = {
	GamepadUSBHostAddonEnabled: yup
		.number()
		.required()
		.label('Gamepad USB Host Add-On Enabled'),
	cyberstickLever: yup
		.number()
		.required()
		.default(0)
		.label('Cyber Stick Lever'),
};

export const gamepadUSBHostState = {
	GamepadUSBHostAddonEnabled: 0,
	cyberstickLever: 0, // 0 = NORMAL
};

const GamepadUSBHost = ({ values, errors, handleChange, handleCheckbox }) => {
	const { getAvailablePeripherals } = useContext(AppContext);
	return (
		<Section title={'Gamepad USB Host Addon'}>
			{getAvailablePeripherals('usb') ? (
				<FormCheck
					label="Enabled"
					type="switch"
					id="GamepadUSBHostAddonButton"
					reverse
					isInvalid={false}
					checked={Boolean(values.GamepadUSBHostAddonEnabled)}
					onChange={(e) => {
						handleCheckbox('GamepadUSBHostAddonEnabled', values);
						handleChange(e);
					}}
				/>
			) : (
				<FormLabel>USB host not enabled!</FormLabel>
			)}
			{/* Cyber Stick Lever select */}
			<FormLabel>Cyberstick options</FormLabel>
			<FormControl
				label="Stick/Lever - Swap position"
				className="form-select-sm"
				groupClassName="col-sm-2 mb-3"
				as="select"
				id="cyberstickLever"
				name="cyberstickLever"
				value={values.cyberstickLever}
				isInvalid={!!errors.cyberstickLever}
				onChange={handleChange}
			>
				<option value={0}>Normal</option>
				<option value={1}>Swap</option>
			</FormControl>
			<FormControl
				label = "Left Lever - Invert Y"
				className="form-select-sm"
				groupClassName="col-sm-2 mb-3"
				as="select"
				id="cyberstickLeverInvertY"
				name="cyberstickLeverInvertY"
				value={values.cyberstickLeverInvertY}
				isInvalid={!!errors.cyberstickLeverInvertY}
				onChange={handleChange}
			>
				<option value={0}>Normal</option>
				<option value={1}>Invert</option>
			</FormControl>
			<FormControl
				label = "Right Stick - Invert Y"
				className="form-select-sm"
				groupClassName="col-sm-2 mb-3"
				as="select"
				id="cyberstickStickInvertY"
				name="cyberstickStickInvertY"
				value={values.cyberstickStickInvertY}
				isInvalid={!!errors.cyberstickStickInvertY}
				onChange={handleChange}
			>
				<option value={0}>Normal</option>
				<option value={1}>Invert</option>
			</FormControl>
			Currently incompatible with Keyboard host addon.
		</Section>
	);
};

export default GamepadUSBHost;
