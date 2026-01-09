import React from 'react';
import type { XRInteractiveProps } from './types';
/**
 * XRCheckbox Props
 */
export interface XRCheckboxProps extends XRInteractiveProps {
    checked?: boolean;
    defaultChecked?: boolean;
    label?: string;
    size?: 'sm' | 'md' | 'lg';
    onChange?: (checked: boolean) => void;
}
/**
 * XRCheckbox - Toggle checkbox component
 *
 * @example
 * ```tsx
 * <XRCheckbox
 *   checked={enabled}
 *   onChange={setEnabled}
 *   label="Enable feature"
 * />
 * ```
 */
export declare function XRCheckbox({ position, rotation, scale, visible, opacity, onMount, onUnmount, disabled, onHover, onHoverEnd, onSelect, checked: controlledChecked, defaultChecked, label, size, onChange, }: XRCheckboxProps): null;
export declare namespace XRCheckbox {
    var displayName: string;
}
/**
 * XRSwitch - Toggle switch variant
 */
export interface XRSwitchProps extends Omit<XRCheckboxProps, 'size'> {
    size?: 'sm' | 'md' | 'lg';
}
export declare function XRSwitch(props: XRSwitchProps): React.JSX.Element;
export declare namespace XRSwitch {
    var displayName: string;
}
/**
 * XRRadio - Radio button component
 */
export interface XRRadioProps extends XRInteractiveProps {
    value: string;
    selectedValue?: string;
    label?: string;
    size?: 'sm' | 'md' | 'lg';
    onSelect?: () => void;
}
export declare function XRRadio({ position, rotation, scale, visible, disabled, value, selectedValue, label, size, onSelect, onMount, onUnmount, }: XRRadioProps): null;
export declare namespace XRRadio {
    var displayName: string;
}
//# sourceMappingURL=XRCheckbox.d.ts.map